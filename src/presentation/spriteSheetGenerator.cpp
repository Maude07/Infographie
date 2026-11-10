#include "presentation/spriteSheetGenerator.h"
#include "domain/shapes/3d/sceneObject3d.h"
#include "domain/shapes/3d/sceneModel3d.h"
#include "infrastructure/spriteLibrary.h"

namespace {
	constexpr float cellFill = 0.9f;
	constexpr float cameraFov = 30.0f;
	constexpr float generatedFps = 12.0f;
}

bool SpriteSheetGenerator::generateRotation(SceneObject & object, int frameCount, int cellSize,
	const string & outputFolder, Result & result) {
	if (frameCount < 1 || cellSize < 8) return false;

	if (!cellFbo.isAllocated() || cellFbo.getWidth() != cellSize) {
		ofFboSettings settings;
		settings.width = settings.height = cellSize;
		settings.internalformat = GL_RGBA;
		settings.useDepth = true;
		cellFbo.allocate(settings);
	}

	const int columns = static_cast<int>(ceil(sqrt(static_cast<float>(frameCount))));
	const int rows = (frameCount + columns - 1) / columns;

	ofPixels sheetPixels;
	sheetPixels.allocate(columns * cellSize, rows * cellSize, OF_PIXELS_RGBA);
	sheetPixels.setColor(ofColor(0, 0, 0, 0));

	const bool is3d = dynamic_cast<SceneObject3d *>(&object) != nullptr;
	ofPixels cellPixels;

	for (int i = 0; i < frameCount; ++i) {
		const float angle = 360.0f * i / frameCount;

		cellFbo.begin();
		ofClear(0, 0, 0, 0);
		if (is3d) renderFrame3d(object, angle, cellSize);
		else renderFrame2d(object, angle, cellSize);
		cellFbo.end();

		cellFbo.readToPixels(cellPixels);
		cellPixels.pasteInto(sheetPixels, (i % columns) * cellSize, (i / columns) * cellSize);
	}

	const string baseName = SpriteLibrary::fileSafeName(object.name);
	const string fileName = baseName + "_" + ofToString(cellSize) + "px_" + ofToString(frameCount)
		+ "f_" + ofGetTimestampString("%Y%m%d-%H%M%S") + ".png";
	const string relativePath = outputFolder + "/" + fileName;

	ofDirectory::createDirectory(outputFolder, true, true);
	if (!ofSaveImage(sheetPixels, relativePath)) {
		ofLogError() << "echec de l'enregistrement de la feuille de sprites: " << relativePath;
		return false;
	}

	result.sheet = make_shared<ofImage>();
	result.sheet->setFromPixels(sheetPixels);

	result.animation = SpriteAnimation();
	result.animation.name = baseName + " (rotation)";
	result.animation.sheetPath = relativePath;
	result.animation.frameWidth = result.animation.frameHeight = cellSize;
	result.animation.columns = columns;
	result.animation.frameCount = frameCount;
	result.animation.fps = generatedFps;

	ofLog() << "<feuille de sprites generee: " << ofToDataPath(relativePath, true) << ">";
	return true;
}

void SpriteSheetGenerator::renderFrame2d(const SceneObject & object, float angle, int cellSize) {
	const ofRectangle bounds = object.getBounds();
	const float diagonal = glm::length(glm::vec2(bounds.width, bounds.height));
	if (diagonal <= 0.0f) return;

	const glm::vec2 center = object.getCenter();
	const float scale = cellSize * cellFill / diagonal;

	ofPushMatrix();
	ofTranslate(cellSize * 0.5f, cellSize * 0.5f);
	ofRotateDeg(object.rotation + angle);
	ofScale(scale, scale);
	ofTranslate(-center.x, -center.y);
	object.draw();
	ofPopMatrix();
}

void SpriteSheetGenerator::renderFrame3d(SceneObject & object, float angle, int cellSize) {
	auto & object3d = dynamic_cast<SceneObject3d &>(object);

	const glm::vec3 center = object3d.getCenter3d();
	float radius = min(object3d.size.x, object3d.size.y) * 0.9f;
	if (auto * model = dynamic_cast<SceneModel3d *>(&object3d)) radius = model->worldBoundingRadius();
	if (radius <= 0.0f) return;

	const float distance = radius / sin(glm::radians(cameraFov * 0.5f)) / cellFill;

	ofCamera camera;
	camera.setVFlip(true);
	camera.setFov(cameraFov);
	camera.setNearClip(max(1.0f, distance - radius * 2.0f));
	camera.setFarClip(distance + radius * 2.0f);
	camera.setPosition(center + glm::vec3(0, 0, distance));
	camera.lookAt(center);

	const float originalRotation = object3d.rotationY;
	object3d.rotationY = originalRotation + angle;

	camera.begin();
	object3d.draw();
	camera.end();

	object3d.rotationY = originalRotation;
}
