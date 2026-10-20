#include "application.h"
#include "domain/scene/sceneImage.h"
#include "domain/shapes/2d/scenePrimitiveEllipse.h"
#include "domain/shapes/2d/scenePrimitiveLine.h"
#include "domain/shapes/2d/scenePrimitivePoint.h"
#include "domain/shapes/2d/scenePrimitiveRect.h"
#include "vector_shapes/sceneShapeFunkyCircles.h"
#include "vector_shapes/sceneShapeStars.h"
#include "vector_shapes/sceneShapeWave.h"
#include "domain/commands/addObjectCommand.h"
#include "domain/commands/removeObjectCommand.h"
#include "domain/commands/parameterCommand.h"
#include "domain/commands/compositeCommand.h"
#include "domain/commands/depthCommand.h"
#include "domain/commands/rotateYCommand.h"
#include "presentation/ui/theme.h"
#include "presentation/ui/defaults.h"

using namespace std;

namespace {
// Button shown in the top-left corner while the panel is hidden
const ofRectangle collapsedGuiButton(10, 10, 30, 30);
const glm::vec2 collapsedGuiIconOffset(10, 18);

	constexpr float importMarginLeft = 50.0f;
	constexpr float importTop = 100.0f;
	constexpr float importCascadeOffset = 30.0f;
	constexpr size_t importCascadeSteps = 10;

	constexpr float histogramMargin = 20.0f;
	constexpr float histogramWidth = 300.0f;
	constexpr float histogramHeight = 120.0f;
	constexpr float histogramMarginBottom = 30.0f;

	constexpr float recordingIndicatorRight = 120.0f;
	constexpr float recordingIndicatorTop = 30.0f;
	constexpr float recordingIndicatorRadius = 8.0f;
	const glm::vec2 recordingLabelOffset(15, 5);

	constexpr float minShapeSize = 3.0f;

	constexpr float modelListGap = 10.0f; // espace entre le panneau et la liste des modeles

	constexpr float rotationStep = 15.0f;
	constexpr float depthStep = 10.0f;
	constexpr float depthLimit = 5000.0f;

	bool isLargeEnough(ToolType type, const glm::vec2 & start, const glm::vec2 & end) {
		glm::vec2 extent = glm::abs(end - start);
		switch (type) {
		case ToolType::Rect:
		case ToolType::Ellipse:
		case ToolType::Stars:
		case ToolType::Wave:
		case ToolType::FunkyCircles:
			return extent.x >= minShapeSize && extent.y >= minShapeSize;
		case ToolType::Line:
			return glm::length(extent) >= minShapeSize;
		case ToolType::Point:
		case ToolType::Bezier:
		case ToolType::Model3d:
		case ToolType::Select:
		case ToolType::Primitive3d:
			return true;
		}
		return true;
	}

	string shapeLabel(ToolType type) {
		switch (type) {
		case ToolType::Rect:
			return "rectangle";
		case ToolType::Line:
			return "ligne";
		case ToolType::Point:
			return "point";
		case ToolType::Ellipse:
			return "ellipse";
		case ToolType::Stars:
			return "etoile";
		case ToolType::Wave:
			return "vague";
		case ToolType::FunkyCircles:
			return "cercles funky";
		case ToolType::Bezier:
			return "bezier";
		case ToolType::Select:
		case ToolType::Model3d:
		case ToolType::Primitive3d:
			break;
		}
		return "objet";
	}

	string primitive3dLabel(Primitive3dType type) {
		switch (type) {
		case Primitive3dType::Cube:
			return "cube";
		case Primitive3dType::Sphere:
			return "sphere";
		case Primitive3dType::Cylinder:
			return "cylindre";
		case Primitive3dType::Cone:
			return "cone";
		}
		return "primitive 3d";
	}

}

void Application::setup() {

#ifdef _WIN32
	FILE * console;
	freopen_s(&console, "CONOUT$", "w", stdout);
	freopen_s(&console, "CONOUT$", "w", stderr);
#endif

	ofSetWindowTitle("interface (u) graphe (i) ");
	ofLog() << "<app::setup>";

	setupTheme();
	renderer.setup();
	depthOfField.setup();
	cursor.setup();

	gui.setup("interface", "setting.json", 0, 0);
	palette = defaults::defaultPalette();

	transformTool.setHistory(&history);

	setupImportGui();
	setupDrawGui();
	setupTransformGui();
	setupExportGui();
	setupMiscGui();
	setupHistoryGui();
	syncColorReferences();

	setupCameraGui();
	for (auto & camera : cameras) camera.reset();
	selectCamera(0);
	placeInspector();
	setupDepthOfFieldGui();
}

void Application::setupTheme() {
	ofxGuiSetDefaultWidth(theme::guiWidth);
	ofxGuiSetDefaultHeight(theme::guiRowHeight);
	ofxGuiSetFillColor(theme::accentColor);
	ofxGuiSetBackgroundColor(theme::panelColor);
	ofxGuiSetBorderColor(theme::panelBorderColor);
	ofxGuiSetHeaderColor(theme::sidebarColor);
	ofxGuiSetTextColor(theme::textColor);
	ofxGuiSetFont(theme::guiFontPath, theme::guiFontSize);
}

void Application::setupImportGui() {
	importButton.setup("Importer une image");
	importButton.addListener(this, &Application::onImportPressed);
	gui.add(&importButton);

	importModelButton.setup("Importer un modele 3D");
	importModelButton.addListener(this, &Application::onImportModelPressed);
	gui.add(&importModelButton);
}

void Application::setupDrawGui() {
	groupDraw.setup("outils de dessin");

	addColorPicker(backgroundSlider, backgroundColor, palettePreviews[0], "couleur du canevas", defaults::defaultBackgroundColor);
	addColorPicker(strokeSlider, strokeColor, palettePreviews[1], "couleur du trait", defaults::defaultStrokeColor);
	addColorPicker(fillSlider, fillColor, palettePreviews[2], "couleur de remplissage", defaults::defaultFillColor);
	groupDraw.add(strokeWeight.set("largeur de la ligne", defaults::defaultStrokeWeight, 0.0f, 10.0f));

	setupVariantGui();
	groupDraw.add(&groupVariants);

	gui.add(&groupDraw);
}

void Application::setupVariantGui() {
	groupVariants.setup("variantes 3D");

	groupVariants.add(variantStretchVertical.set("etirement vertical", false));
	groupVariants.add(variantVerticalFactor.set("degre vertical", defaults::defaultStretchFactor, defaults::minStretchFactor, defaults::maxStretchFactor));

	groupVariants.add(variantStretchHorizontal.set("etirement horizontal", false));
	groupVariants.add(variantHorizontalFactor.set("degre horizontal", defaults::defaultStretchFactor, defaults::minStretchFactor, defaults::maxStretchFactor));

	groupVariants.add(variantTinted.set("teinte", false));
	variantTintColor.set("couleur de teinte", defaults::defaultTintColor, ofColor(0, 0), ofColor(255, 255));
	variantTintSlider.setup(variantTintColor);
	palettePreviews[3].setup(palette, variantTintColor, groupVariants.getWidth());
	variantTintSlider.add(&palettePreviews[3]);
	groupVariants.add(&variantTintSlider);
	variantTintColor.addListener(this, &Application::onColorChanged);

	groupVariants.add(variantMetallic.set("metallique", false));
	groupVariants.add(variantMetalness.set("degre metallique", defaults::defaultMetalness, 0.0f, 1.0f));
}

void Application::resetVariantGui() {
	variantStretchVertical = false;
	variantVerticalFactor = defaults::defaultStretchFactor;
	variantStretchHorizontal = false;
	variantHorizontalFactor = defaults::defaultStretchFactor;
	variantTinted = false;
	variantTintColor = defaults::defaultTintColor;
	variantMetallic = false;
	variantMetalness = defaults::defaultMetalness;
}

ModelVariant Application::variantFromGui() const {
	ModelVariant v;
	v.stretchVertical = variantStretchVertical;
	v.verticalFactor = variantVerticalFactor;
	v.stretchHorizontal = variantStretchHorizontal;
	v.horizontalFactor = variantHorizontalFactor;
	v.tinted = variantTinted;
	v.tint = variantTintColor;
	v.metallic = variantMetallic;
	v.metalness = variantMetalness;
	return v;
}

void Application::setupTransformGui() {
	inspector.setup("inspecteur", "inspector.json", 0, 0);
	groupTransform.setup("transformation");
	groupTransform.add(transformX.set("x", 0.0f, 0.0f, 2000.0f));
	groupTransform.add(transformY.set("y", 0.0f, 0.0f, 2000.0f));
	groupTransform.add(transformRotation.set("rotation", 0.0f, -180.0f, 180.0f));
	groupTransform.add(transformWidth.set("largeur", 100.0f, SceneObject::minSize, 2000.0f));
	groupTransform.add(transformHeight.set("hauteur", 100.0f, SceneObject::minSize, 2000.0f));

	for (auto * parameter : transformParameters()) {
		parameter->addListener(this, &Application::onTransformChanged);
	}
	inspector.add(&groupTransform);
}

void Application::placeInspector() {
	inspector.setPosition(ofGetWidth() - inspector.getWidth(), 0);
}

void Application::addColorPicker(ofxColorSlider & slider, ofParameter<ofColor> & color, PalettePreview & preview,
	const string & name, const ofColor & initialColor) {
	color.set(name, initialColor, ofColor(0, 0), ofColor(255, 255));
	slider.setup(color);
	preview.setup(palette, color, groupDraw.getWidth());
	slider.add(&preview);
	groupDraw.add(&slider);
	color.addListener(this, &Application::onColorChanged);
}

glm::vec2 Application::modelListOrigin() const {
	return glm::vec2(0.0f, gui.getShape().getBottom() + modelListGap);
}

// Hauteur totale du contenu de la colonne de gauche : le panneau ofxGui
// plus la liste des modeles 3D importes dessinee juste en dessous.
float Application::sidebarContentHeight() const {
	return gui.getHeight() + modelListGap + modelListPanel.getHeight(modelLibrary.ids().size());
}

void Application::setupExportGui() {
	groupExport.setup("exportation d'images");
	groupExport.add(exportFps.set("images/sec", 24, 1, 60));
	groupExport.add(exportDuration.set("duree (s, 0=illimite)", 5.0f, 0.0f, 60.0f));

	recordButton.setup("enregistrer / arreter (r)");
	recordButton.addListener(this, &Application::onRecordPressed);
	groupExport.add(&recordButton);

	gui.add(&groupExport);
}

void Application::setupMiscGui() {
	gui.add(displayText.set("text", defaults::defaultText));

	resetButton.setup("Reinitialiser");
	resetButton.addListener(this, &Application::onResetPressed);
	gui.add(&resetButton);

	gui.add(showGui.set("visible", true));
	gui.add(showSceneTree.set("graphe de scene (i)", false));

	histogramButton.setup("Calculer l'histogramme");
	histogramButton.addListener(this, &Application::onHistogramPressed);
	gui.add(&histogramButton);
}

void Application::update() {
	renderer.backgroundColor = backgroundColor;
	renderer.strokeColor = strokeColor;
	renderer.strokeWeight = strokeWeight;
	renderer.text = displayText;
	renderer.update();
	syncTransformGui();
	sceneTreePanel.rightOffset = ofGetWidth() - canvasRight();
	transformTool.setCanvas(ofRectangle(canvasLeft(), 0, canvasRight() - canvasLeft(), ofGetHeight()));

	cameraController.update(ofGetLastFrameTime());
	activeCamera().setFov(cameraFov);
	activeCamera().setClip(cameraNear, cameraFar);
	activeCamera().setOrtho(cameraOrtho);
	syncModelCameras();

	currentVariant = variantFromGui();

	bool mouseDown = ofGetMousePressed();

	if (!mouseDown && wasMousePressedLastFrame) {
		commitColorChanges();
	}

	if (!mouseDown) {
		syncColorReferences();
		commitTransformChanges();
	}

	wasMousePressedLastFrame = mouseDown;
}

void Application::updateSidebarScroll() {
	float contentHeight = sidebarContentHeight();
	float visibleHeight = static_cast<float>(ofGetHeight());

	maxScroll = max(0.0f, contentHeight - visibleHeight);
	scrollOffset = ofClamp(scrollOffset, 0.0f, maxScroll);

	gui.setPosition(0, -scrollOffset);
}

void Application::drawSidebarScrollbar() const {
	float trackHeight = static_cast<float>(ofGetHeight());
	float contentHeight = max(sidebarContentHeight(), trackHeight);
	float trackX = gui.getWidth() + (scrollbarTrackWidth - scrollbarThumbWidth) * 0.5f;

	float thumbHeight = ofClamp(trackHeight * (trackHeight / contentHeight), 20.0f, trackHeight);
	float thumbY = maxScroll > 0.0f ? (scrollOffset / maxScroll) * (trackHeight - thumbHeight) : 0.0f;

	ofPushStyle();
	ofFill();
	ofSetColor(theme::panelColor);
	ofDrawRectangle(trackX, 0, scrollbarThumbWidth, trackHeight);

	ofSetColor(maxScroll > 0.0f ? theme::accentColor : theme::panelBorderColor);
	ofDrawRectangle(trackX, thumbY, scrollbarThumbWidth, thumbHeight);
	ofPopStyle();
}

bool Application::isOverScrollbarThumb(int x, int y) const {
	if (maxScroll <= 0.0f) return false;

	float trackHeight = static_cast<float>(ofGetHeight());
	float contentHeight = max(sidebarContentHeight(), trackHeight);
	float trackX = gui.getWidth() + (scrollbarTrackWidth - scrollbarThumbWidth) * 0.5f;

	float thumbHeight = ofClamp(trackHeight * (trackHeight / contentHeight), 20.0f, trackHeight);
	float thumbY = (scrollOffset / maxScroll) * (trackHeight - thumbHeight);

	return x >= trackX && x <= trackX + scrollbarThumbWidth
		&& y >= thumbY && y <= thumbY + thumbHeight;
}

void Application::setScrollFromThumbY(float thumbScreenY) {
	float trackHeight = static_cast<float>(ofGetHeight());
	float contentHeight = max(sidebarContentHeight(), trackHeight);
	float thumbHeight = ofClamp(trackHeight * (trackHeight / contentHeight), 20.0f, trackHeight);

	float usableTrack = trackHeight - thumbHeight;
	if (usableTrack <= 0.0f) return;

	float ratio = ofClamp((thumbScreenY - scrollbarDragOffsetY) / usableTrack, 0.0f, 1.0f);
	scrollOffset = ratio * maxScroll;
}

void Application::setupHistoryGui() {
	groupHistory.setup("historique");

	buttonUndo.setup("annuler (ctrl+z)");
	buttonUndo.addListener(this, &Application::buttonUndoPressed);
	groupHistory.add(&buttonUndo);

	buttonRedo.setup("refaire (ctrl+y)");
	buttonRedo.addListener(this, &Application::buttonRedoPressed);
	groupHistory.add(&buttonRedo);

	gui.add(&groupHistory);
}

void Application::buttonUndoPressed() {
	if (history.canUndo()) {
		history.undo();
		transformTool.clearSelection();
		syncColorReferences();
	}
}

void Application::buttonRedoPressed() {
	if (history.canRedo()) {
		history.redo();
		transformTool.clearSelection();
		syncColorReferences();
	}
}

void Application::syncColorReferences() {
	colorRefBackground = backgroundColor;
	colorRefStroke = strokeColor;
	colorRefFill = fillColor;
}

void Application::commitColorChanges() {
	vector<unique_ptr<Command>> changes;

	if (backgroundColor.get() != colorRefBackground) {
		changes.push_back(make_unique<ParameterCommand<ofColor>>(backgroundColor, colorRefBackground, backgroundColor.get()));
	}
	if (strokeColor.get() != colorRefStroke) {
		changes.push_back(make_unique<ParameterCommand<ofColor>>(strokeColor, colorRefStroke, strokeColor.get()));
	}
	if (fillColor.get() != colorRefFill) {
		changes.push_back(make_unique<ParameterCommand<ofColor>>(fillColor, colorRefFill, fillColor.get()));
	}

	if (changes.empty()) return;

	if (changes.size() == 1) {
		history.push(move(changes.front()));
	} else {
		history.push(make_unique<CompositeCommand>(move(changes)));
	}
}

void Application::draw() {
	float left = canvasLeft();
	float right = canvasRight();
	ofRectangle canvas(left, 0, right - left, ofGetHeight());

	renderer.visibleOffsetX = left;
	renderer.visibleRightX = right;
	renderer.draw(scene, activeCamera());
	if (depthOfFieldEnabled) {
		depthOfField.begin();
		renderer.draw(scene, activeCamera());
		depthOfField.end();
		depthOfField.draw(activeCamera(), focusDistance, focusRange, maxBlur);
	} else {
		renderer.draw(scene, activeCamera());
	}

	if (histogramRequested) {
		computeHistogram(canvas);
	}

	exporter.captureX = static_cast<int>(canvas.x);
	exporter.captureY = static_cast<int>(canvas.y);
	exporter.captureWidth = static_cast<int>(canvas.width);
	exporter.captureHeight = static_cast<int>(canvas.height);
	exporter.capture();

	if (showGui) {
		drawSidebarBackground();
	}

	transformTool.drawOverlay();
	drawCameraLabel();

	if (isMouseButtonPressed) {
		if (toolType == ToolType::Bezier && bezierStrokeTool.isActive()) {
			bezierStrokeTool.draw();
		} else if (toolType == ToolType::Model3d) {
			if (auto preview = makeModel(mousePressPos, mouseCurrentPos)) {
				activeCamera().begin();
				preview->render();
				activeCamera().end();
			}
		} else if (toolType == ToolType::Primitive3d) {
			auto preview = makePrimitive3d(mousePressPos, mouseCurrentPos);
			activeCamera().begin();
			preview->render();
			activeCamera().end();
		} else if (auto preview = makeShape(toolType, mousePressPos, mouseCurrentPos)) {
			preview->draw();
		}
	}

	if (exporter.isRecording) {
		drawRecordingIndicator();
	}

	if (toolType == ToolType::Model3d) {
		ofDrawBitmapString("variante: " + currentVariant.label(),
			canvasLeft() + 10, ofGetHeight() - 10);
	}

	if (histogram.is_computed()) {
		histogram.draw(ofRectangle(left + histogramMargin, ofGetHeight() - histogramMarginBottom - histogramHeight,
			histogramWidth, histogramHeight));
	}

	if (showSceneTree) {
		sceneTreePanel.draw(scene, transformTool.getSelection(), glm::vec2(ofGetMouseX(), ofGetMouseY()));
	}

	buttonUndo.setBackgroundColor(history.canUndo() ? ofColor(90, 140, 240) : ofColor(50, 50, 55));
	buttonRedo.setBackgroundColor(history.canRedo() ? ofColor(90, 140, 240) : ofColor(50, 50, 55));

	if (showGui) {
		updateSidebarScroll();

		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, static_cast<int>(gui.getWidth()), ofGetHeight());

		gui.draw();

		glDisable(GL_SCISSOR_TEST);

		drawSidebarScrollbar();

		inspector.draw();
		modelListPanel.draw(modelLibrary.ids(),
			toolType == ToolType::Model3d ? currentModelId : "",
			modelListOrigin(), gui.getWidth(), glm::vec2(ofGetMouseX(), ofGetMouseY()));
	} else {
		drawCollapsedGuiButton();
	}

	glm::vec2 mouse(ofGetMouseX(), ofGetMouseY());
	cursor.draw(currentCursorState(mouse), mouse);
}

void Application::computeHistogram(const ofRectangle & canvas) {
	histogramRequested = false;

	ofImage capture;
	capture.grabScreen(static_cast<int>(canvas.x), static_cast<int>(canvas.y),
		static_cast<int>(canvas.width), static_cast<int>(canvas.height));

	if (!capture.isAllocated()) {
		ofLogWarning() << "histogramme: capture échouée";
		return;
	}
	histogram.compute(capture);
	ofLog() << "histogramme calculé";
}

void Application::syncTransformGui() {
	const auto & selected = transformTool.getSelection();
	string title = selected.size() == 1 ? "inspecteur : " + (*selected.begin())->name : "inspecteur";
	if (inspector.getName() != title) inspector.setName(title);

	if (selected.size() != 1) return;
	SceneObject * selection = *selected.begin();

	ofRectangle bounds = selection->getBounds();
	glm::vec2 center = selection->getCenter();

	isSyncingTransformGui = true;
	transformX = center.x;
	transformY = center.y;
	transformRotation = selection->rotation;
	transformWidth = bounds.width;
	transformHeight = bounds.height;
	isSyncingTransformGui = false;
}

void Application::drawSidebarBackground() const {
	ofPushStyle();
	ofFill();
	ofSetColor(theme::sidebarColor);
	ofDrawRectangle(0, 0, gui.getWidth(), ofGetHeight());
	ofDrawRectangle(canvasRight(), 0, ofGetWidth() - canvasRight(), ofGetHeight());
	ofSetColor(theme::sidebarBorderColor);
	ofDrawLine(gui.getWidth(), 0, gui.getWidth(), ofGetHeight());
	ofDrawLine(canvasRight(), 0, canvasRight(), ofGetHeight());
	ofPopStyle();
}

void Application::drawCollapsedGuiButton() const {
	ofPushStyle();
	ofFill();
	ofSetColor(theme::panelColor);
	ofDrawRectangle(collapsedGuiButton);

	ofNoFill();
	ofSetColor(theme::accentColor);
	ofSetLineWidth(2);
	ofDrawRectangle(collapsedGuiButton);

	ofFill();
	ofSetColor(theme::textColor);
	ofDrawBitmapString(">", collapsedGuiButton.getPosition() + glm::vec3(collapsedGuiIconOffset, 0));
	ofPopStyle();
}

void Application::drawRecordingIndicator() const {
	glm::vec2 center(canvasRight() - recordingIndicatorRight, recordingIndicatorTop);

	ofPushStyle();
	ofFill();
	ofSetColor(255, 0, 0);
	ofDrawCircle(center, recordingIndicatorRadius);
	ofSetColor(255);
	ofDrawBitmapString("REC " + ofToString(exporter.frameCount), center + recordingLabelOffset);
	ofPopStyle();
}

CursorState Application::currentCursorState(const glm::vec2 & mouse) const {
	if (showGui && mouse.x <= canvasLeft()) return CursorState::Default;
	if (showGui && gui.getShape().inside(mouse)) return CursorState::Default;
	if (showGui && inspector.getShape().inside(mouse)) return CursorState::Default;
	if (!showGui && collapsedGuiButton.inside(mouse)) return CursorState::Default;
	if (showSceneTree && sceneTreePanel.hitTest(scene, mouse.x, mouse.y)) return CursorState::Hover;

	switch (toolType) {
	case ToolType::Rect:
	case ToolType::Line:
	case ToolType::Ellipse:
	case ToolType::Stars:
	case ToolType::Wave:
	case ToolType::FunkyCircles:
	case ToolType::Bezier:
	case ToolType::Model3d:
	case ToolType::Primitive3d:
		return CursorState::Crosshair;
	case ToolType::Point:
		return CursorState::Point;
	case ToolType::Select:
		break;
	}

	if (transformTool.isResizingSelection() || transformTool.isOverHandle(mouse.x, mouse.y)) {
		return CursorState::Resize;
	}
	if (isMouseButtonPressed && transformTool.hasSelection()) return CursorState::Move;
	if (scene.hitTest(mouse.x, mouse.y)) return CursorState::Hover;

	return CursorState::Default;
}

void Application::onColorChanged(ofColor & color) {
	lastActiveColor = color;
}

void Application::keyPressed(ofKeyEventArgs & args) {
	if (args.key == OF_KEY_LEFT || args.key == OF_KEY_RIGHT) {
		rotateSelectedModels(args.key == OF_KEY_LEFT ? -rotationStep : rotationStep);
		return;
	}

	if (args.key == OF_KEY_UP || args.key == OF_KEY_DOWN) {
		float step = depthStep;
		moveSelectedModelsDepth(args.key == OF_KEY_UP ? step : -step);
		return;
	}

	if (args.isRepeat) return;

	cameraController.keyPressed(args.key);

	switch (args.key) {
	case 'u':
		showGui = !showGui;
		ofLog() << "<toggle ui: " << showGui << ">";
		break;

	case 'i':
		showSceneTree = !showSceneTree;
		ofLog() << "<toggle scene tree: " << showSceneTree << ">";
		break;

	case 'r':
		onRecordPressed();
		break;

	case '1':
		setToolType(ToolType::Select);
		break;
	case '2':
		setToolType(ToolType::Rect);
		break;
	case '3':
		setToolType(ToolType::Line);
		break;
	case '4':
		setToolType(ToolType::Point);
		break;
	case '5':
		setToolType(ToolType::Ellipse);
		break;
	case 'q':
		this->toggleBoundingBoxes();
		break;
	case 'm':
		if (toolType == ToolType::Model3d) {
			selectNextModel();
		} else if (!currentModelId.empty()) {
			setToolType(ToolType::Model3d);
		} else {
			ofLog() << "<aucun modele 3d importe>";
		}
		break;
	case '6':
		setToolType(ToolType::Stars);
		break;
	case '7':
		setToolType(ToolType::Wave);
		break;
	case '8':
		setToolType(ToolType::FunkyCircles);
		break;
	case '9':
		if (toolType == ToolType::Primitive3d) {
			selectPrimitive3d(static_cast<Primitive3dType>((static_cast<int>(currentPrimitive3d) + 1) % 4));
		} else {
			selectPrimitive3d(currentPrimitive3d);
		}
		break;
	case 'b':
		setToolType(ToolType::Bezier);
		break;

	case 'z':
	case 26:
		if (ofGetKeyPressed(OF_KEY_CONTROL) || args.key == 26) {
			history.undo();
			transformTool.clearSelection();
			syncColorReferences();
		}
		break;

	case 'y':
	case 25:
		if (ofGetKeyPressed(OF_KEY_CONTROL) || args.key == 25) {
			history.redo();
			transformTool.clearSelection();
			syncColorReferences();
		}
		break;

	case OF_KEY_DEL:
	case OF_KEY_BACKSPACE:
		if (transformTool.hasSelection()) {
			deleteSelectedObject();
		} else {
			removeSelectedPaletteColor();
		}
		break;

	case OF_KEY_RETURN:
		palette.push_back(lastActiveColor);
		ofLog() << "<added color: " << lastActiveColor << ">";
		break;
	case 'c':
		resetCamera();
		break;
	case 'f':
		frameCameraOnObject();
		break;
	case OF_KEY_F1:
	case OF_KEY_F2:
	case OF_KEY_F3:
	case OF_KEY_F4:
	case OF_KEY_F5:
	case OF_KEY_F6:
		isolateSelection();
		selectCamera(static_cast<size_t>(args.key - OF_KEY_F1));
		break;
	case '0':
		showWholeScene();
		break;
	case 'p':
		cameraOrtho = !cameraOrtho;
		break;
	}
}

void Application::keyReleased(ofKeyEventArgs & args) {
	cameraController.keyReleased(args.key);
}

void Application::deleteSelectedObject() {
	auto toRemove = transformTool.getSelection();
	ofLog() << "<deleted objects: " << toRemove.size() << ">";
	transformTool.clearSelection();
	history.execute(make_unique<RemoveObjectsCommand>(&scene, toRemove));
}

void Application::setToolType(ToolType mode) {
	toolType = mode;
	transformTool.clearSelection();
	isMouseButtonPressed = false;
	bezierStrokeTool.cancel();
}

void Application::removeSelectedPaletteColor() {

	for (const auto & preview : palettePreviews) {
		int index = preview.selectedIndex;
		if (index < 0 || index >= static_cast<int>(palette.size())) continue;

		palette.erase(palette.begin() + index);
		ofLog() << "<deleted color at index: " << index << ">";
		break;
	}

	clearPaletteSelection();
}

void Application::clearPaletteSelection() {
	for (auto & preview : palettePreviews) {
		preview.selectedIndex = -1;
	}
}

void Application::mousePressed(int x, int y, int button) {
	if (button != OF_MOUSE_BUTTON_LEFT) {
		cameraController.mousePressed(x, y, button, isOverUi(x, y));
		return;
	}

	if (!showGui && collapsedGuiButton.inside(x, y)) {
		showGui = true;
		return;
	}

	if (showGui && isOverScrollbarThumb(x, y)) {
		isDraggingScrollbar = true;

		float trackHeight = static_cast<float>(ofGetHeight());
		float contentHeight = max(sidebarContentHeight(), trackHeight);
		float thumbHeight = ofClamp(trackHeight * (trackHeight / contentHeight), 20.0f, trackHeight);
		float thumbY = (scrollOffset / maxScroll) * (trackHeight - thumbHeight);

		scrollbarDragOffsetY = y - thumbY;
		return;
	}

	if (showGui && x <= canvasLeft()) return;

	if (showGui) {
		string clicked = modelListPanel.hitTest(modelLibrary.ids(), modelListOrigin(), gui.getWidth(), x, y);
		if (!clicked.empty()) {
			currentModelId = clicked;
			setToolType(ToolType::Model3d);
			return;
		}
	}

	bool additive = ofGetKeyPressed(OF_KEY_SHIFT) || ofGetKeyPressed(OF_KEY_CONTROL);

	if (showSceneTree) {
		if (SceneObject * clicked = sceneTreePanel.hitTest(scene, x, y)) {
			if (toolType != ToolType::Select) setToolType(ToolType::Select);
			transformTool.select(clicked, additive);
			return;
		}
	}

	isMouseButtonPressed = true;

	if (toolType == ToolType::Select) {
		transformTool.mousePressed(scene, x, y, additive);
		return;
	}

	mousePressPos = mouseCurrentPos = glm::vec2(x, y);
	if (toolType == ToolType::Bezier) {
		bezierStrokeTool.begin(mousePressPos, fillColor, strokeColor, strokeWeight,
			shapeLabel(ToolType::Bezier));
	}
}

void Application::mouseDragged(int x, int y, int button) {
	if (isDraggingScrollbar) {
		setScrollFromThumbY(static_cast<float>(y));
		return;
	}
	if (button != OF_MOUSE_BUTTON_LEFT) {
		cameraController.mouseDragged(x, y, button);
		return;
	}

	if (!isMouseButtonPressed) return;

	if (toolType == ToolType::Select) {
		transformTool.mouseDragged(x, y);
		return;
	}
	mouseCurrentPos = glm::vec2(x, y);
	if (toolType == ToolType::Bezier) {
		bezierStrokeTool.extend(mouseCurrentPos);
	}
}

void Application::mouseReleased(int x, int y, int button) {
	if (isDraggingScrollbar) {
		isDraggingScrollbar = false;
		return;
	}
	if (button != OF_MOUSE_BUTTON_LEFT) {
		cameraController.mouseReleased(x, y, button);
		return;
	}

	if (!isMouseButtonPressed) return;
	isMouseButtonPressed = false;

	if (toolType == ToolType::Select) {
		transformTool.mouseReleased(scene);
		return;
	}
	mouseCurrentPos = glm::vec2(x, y);
	if (toolType == ToolType::Bezier) {
		if (auto curve = bezierStrokeTool.commit(mouseCurrentPos)) {
			history.execute(make_unique<AddObjectCommand>(&scene, move(curve)));
		}
		return;
	}

	if (toolType == ToolType::Model3d) {
		addModel();
		return;
	}

	if (toolType == ToolType::Primitive3d) {
		addPrimitive3d();
		return;
	}

	addVectorShape();
}

void Application::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	if (showGui && x < canvasLeft()) {
		scrollOffset = ofClamp(scrollOffset - scrollY * scrollSpeed, 0.0f, maxScroll);
		return;
	}
	if (isOverUi(x, y)) return;

	if (ofGetKeyPressed(OF_KEY_ALT) && scrollY != 0.f) {
		float step = depthStep;
		if (moveSelectedModelsDepth(scrollY > 0.f ? step : -step)) return;
	}
	cameraController.mouseScrolled(scrollY);
}

void Application::onResetPressed() {
	if (exporter.isRecording) exporter.stop();
	setToolType(ToolType::Select);
	scene.clear();
	histogram.reset();
	histogramRequested = false;
	history.clear();
	syncColorReferences();

	displayText.set(defaults::defaultText);
	backgroundColor = defaults::defaultBackgroundColor;
	strokeColor = defaults::defaultStrokeColor;
	fillColor = defaults::defaultFillColor;
	strokeWeight = defaults::defaultStrokeWeight;

	resetVariantGui();

	palette = defaults::defaultPalette();
	clearPaletteSelection();

	ofLog() << "button pressed>";
}

void Application::onImportPressed() {
	ofFileDialogResult result = ofSystemLoadDialog("Choisir une image");
	if (!result.bSuccess) return;

	auto image = SceneImage::load(result.getPath());
	if (!image) return;

	float offset = importCascadeOffset * (scene.size() % importCascadeSteps);
	image->position = { canvasLeft() + importMarginLeft + offset, importTop + offset };
	history.execute(make_unique<AddObjectCommand>(&scene, move(image)));
}

void Application::onRecordPressed() {
	exporter.fps = exportFps;
	exporter.duration = exportDuration;
	exporter.toggle();
}

void Application::onHistogramPressed() {
	histogramRequested = true;
}

void Application::onImportModelPressed() {
	ofFileDialogResult result = ofSystemLoadDialog("Choisir un modele 3D", false, ofToDataPath("models", true));
	if (!result.bSuccess) return;

	string id = ofFilePath::getBaseName(result.getPath());
	if (!modelLibrary.load(id, result.getPath())) return;

	currentModelId = id;
	setToolType(ToolType::Model3d);
}

void Application::onTransformChanged(float &) {
	if (isSyncingTransformGui) return;

	const auto & selected = transformTool.getSelection();
	if (selected.size() != 1) return;
	SceneObject * selection = *selected.begin();
	if (transformEditTarget != selection) {
		commitTransformChanges();
		transformEditTarget = selection;
		transformEditStart = TransformState::of(*selection);
	}

	ofRectangle bounds = selection->getBounds();
	float width = selection->isResizable() ? max(SceneObject::minSize, transformWidth.get()) : bounds.width;
	float height = selection->isResizable() ? max(SceneObject::minSize, transformHeight.get()) : bounds.height;
	if (selection->isSquare()) {
		float side = width != bounds.width ? width : height;
		width = side;
		height = side;
	}
	bool widthChanged = width != bounds.width;
	bool heightChanged = height != bounds.height;
	bounds.setFromCenter(transformX, transformY, width, height);
	selection->setBounds(bounds);

	if (selection->isRotatable()) {
		selection->rotation = transformRotation;
	}
	transformTool.fitInCanvas(*selection, widthChanged, heightChanged);
}

void Application::commitTransformChanges() {
	if (!transformEditTarget) return;
	TransformState current = TransformState::of(*transformEditTarget);
	if (current != transformEditStart) {
		history.push(make_unique<TransformCommand>(transformEditTarget, transformEditStart, current));
	}
	transformEditTarget = nullptr;
}

void Application::windowResized(int w, int h) {
	ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
	placeInspector();
}

unique_ptr<ScenePrimitive> Application::makeShape(ToolType type, const glm::vec2 & start,
	 const glm::vec2 & end) const {
	unique_ptr<ScenePrimitive> shape;

	switch (type) {
	case ToolType::Line:
		shape = make_unique<ScenePrimitiveLine>();
		shape->SceneObject::position = start;
		shape->size = end;
		shape->filled = false;
		break;
	case ToolType::Point:
		shape = make_unique<ScenePrimitivePoint>();
		shape->SceneObject::position = start;
		shape->size = end;
		shape->filled = false;
		break;
	case ToolType::Rect:
		shape = make_unique<ScenePrimitiveRect>();
		shape->SceneObject::position = glm::min(start, end);
		shape->size = glm::abs(end - start);
		break;
	case ToolType::Ellipse:
		shape = make_unique<ScenePrimitiveEllipse>();
		shape->SceneObject::position = (start + end) * 0.5f;
		shape->size = glm::abs(end - start);
		break;
	case ToolType::Stars:
		shape = make_unique<SceneShapeStars>();
		shape->SceneObject::position = (start + end) * 0.5f;
		shape->size = glm::abs(end - start);
		break;
	case ToolType::Wave:
		shape = make_unique<SceneShapeWave>();
		shape->SceneObject::position = (start + end) * 0.5f;
		shape->size = glm::abs(end - start);
		break;
	case ToolType::FunkyCircles:
		shape = make_unique<SceneShapeFunkyCircles>();
		shape->SceneObject::position = (start + end) * 0.5f;
		shape->size = glm::abs(end - start);
		break;
	case ToolType::Bezier:
	case ToolType::Model3d:
	case ToolType::Primitive3d:
	case ToolType::Select:
		return nullptr;
	}

	shape->name = shapeLabel(type);
	applyDrawStyle(*shape);
	return shape;
}

void Application::addVectorShape() {
	if (!isLargeEnough(toolType, mousePressPos, mouseCurrentPos)) return;

	if (auto shape = makeShape(toolType, mousePressPos, mouseCurrentPos)) {
		history.execute(make_unique<AddObjectCommand>(&scene, move(shape)));
	}
}

void Application::applyDrawStyle(ScenePrimitive & primitive) const {
	primitive.fillColor = fillColor;
	primitive.lineColor = strokeColor;
	primitive.lineWidth = strokeWeight;
}

unique_ptr<SceneModel3d> Application::makeModel(const glm::vec2 & start, const glm::vec2 & end) const {
	auto model = modelLibrary.get(currentModelId);
	if (!model) return nullptr;

	glm::vec2 worldStart = activeCamera().screenToPlane(start, 0.f);
	glm::vec2 worldEnd = activeCamera().screenToPlane(end, 0.f);
	glm::vec2 extent = glm::abs(worldEnd - worldStart);
	glm::vec2 center = (worldStart + worldEnd) * 0.5f;
	float footprint = max(extent.x, extent.y);
	if (glm::length(end - start) < minShapeSize) footprint = defaults::defaultModelSize;

	auto instance = make_unique<SceneModel3d>(currentModelId, model, center, footprint, currentVariant);
	instance->camera = &activeCamera();
	return instance;
}

void Application::addModel() {
	if (auto instance = makeModel(mousePressPos, mouseCurrentPos)) {
		history.execute(make_unique<AddObjectCommand>(&scene, move(instance)));
	}
}

void Application::selectNextModel() {
	auto ids = modelLibrary.ids();
	if (ids.empty()) return;

	auto it = find(ids.begin(), ids.end(), currentModelId);
	size_t next = (it == ids.end()) ? 0 : (it - ids.begin() + 1) % ids.size();
	currentModelId = ids[next];
	setToolType(ToolType::Model3d);
	ofLog() << "<model: " << currentModelId << ">";
}

void Application::toggleBoundingBoxes() {
	for (auto & object : scene.getObjects()) {
		if (auto * model = dynamic_cast<SceneModel3d *>(object.get())) {
			model->bShowBoundingBox = !model->bShowBoundingBox;
		}
	}
}

bool Application::moveSelectedModelsDepth(float delta) {
	vector<DepthCommand::Change> changes;
	for (SceneObject * object : transformTool.getSelection()) {
		if (auto * model = dynamic_cast<SceneObject3d *>(object)) {
			float newDepth = ofClamp(model->depth + delta, -depthLimit, depthLimit);
			if (newDepth != model->depth) {
				changes.push_back({ model, model->depth, newDepth });
			}
		}
	}

	sort(changes.begin(), changes.end(),
		[](const DepthCommand::Change & a, const DepthCommand::Change & b) { return a.model < b.model; });

	bool hadModels = !changes.empty();
	if (!hadModels) {
		for (SceneObject * object : transformTool.getSelection()) {
			if (dynamic_cast<SceneObject3d *>(object)) return true;
		}
		return false;
	}

	for (auto & c : changes) c.model->depth = c.newDepth;

	auto * last = dynamic_cast<DepthCommand *>(history.lastUndo());
	if (!last || history.canRedo() || !last->tryMerge(changes)) {
		history.push(make_unique<DepthCommand>(move(changes)));
	}
	ofLog() << "<profondeur: " << delta << ">";
	return true;
}

void Application::rotateSelectedModels(float degrees) {
	vector<unique_ptr<Command>> changes;

	for (SceneObject * object : transformTool.getSelection()) {
		auto * object3d = dynamic_cast<SceneObject3d *>(object);
		if (!object3d) continue;
		float * rotationY = &object3d->rotationY;

		float newValue = fmod(*rotationY + degrees, 360.0f);
		changes.push_back(make_unique<RotateYCommand>(*rotationY, *rotationY, newValue));
	}

	if (changes.empty()) return;

	if (changes.size() == 1) {
		history.execute(move(changes.front()));
	} else {
		history.execute(make_unique<CompositeCommand>(move(changes)));
	}
}

unique_ptr<ScenePrimitive3d> Application::makePrimitive3d(const glm::vec2 & start, const glm::vec2 & end) const {
	unique_ptr<ScenePrimitive3d> primitive;

	switch (currentPrimitive3d) {
	case Primitive3dType::Cube:
		primitive = make_unique<ScenePrimitiveCube>();
		break;
	case Primitive3dType::Sphere:
		primitive = make_unique<ScenePrimitiveSphere>();
		break;
	case Primitive3dType::Cylinder:
		primitive = make_unique<ScenePrimitiveCylinder>();
		break;
	case Primitive3dType::Cone:
		primitive = make_unique<ScenePrimitiveCone>();
		break;
	}

	glm::vec2 worldStart = activeCamera().screenToPlane(start, 0.f);
	glm::vec2 worldEnd = activeCamera().screenToPlane(end, 0.f);
	glm::vec2 extent = glm::abs(worldEnd - worldStart);
	float footprint = max(extent.x, extent.y);
	if (glm::length(end - start) < minShapeSize) footprint = defaults::defaultModelSize;

	primitive->size = { footprint, footprint };
	primitive->position = (worldStart + worldEnd) * 0.5f - primitive->size * 0.5f;
	primitive->camera = &activeCamera();
	primitive->name = primitive3dLabel(currentPrimitive3d);
	primitive->color = fillColor;
	return primitive;
}


void Application::addPrimitive3d() {
	history.execute(make_unique<AddObjectCommand>(&scene, makePrimitive3d(mousePressPos, mouseCurrentPos)));
}

void Application::selectPrimitive3d(Primitive3dType type) {
	currentPrimitive3d = type;
	setToolType(ToolType::Primitive3d);
	ofLog() << "<primitive 3d: " << primitive3dLabel(type) << ">";
}

void Application::setupCameraGui() {
	groupCamera.setup("cameras");
	groupCamera.add(cameraSelector.set("camera active (F1-F6)", 1, 1, static_cast<int>(cameraCount)));
	cameraNameLabel.set("vue", cameras[0].getName());
	cameraNameLabelGui.setup(cameraNameLabel);
	groupCamera.add(&cameraNameLabelGui);
	cameraSelector.addListener(this, &Application::onCameraSelectorChanged);
	groupCamera.add(cameraFov.set("champ de vision (deg)", SceneCamera::DEFAULT_FOV, 10.f, 120.f));
	groupCamera.add(cameraNear.set("plan proche", SceneCamera::DEFAULT_NEAR, 0.1f, 3000.f));
	groupCamera.add(cameraFar.set("plan eloigne", SceneCamera::DEFAULT_FAR, 10.f, 20000.f));
	groupCamera.add(cameraOrtho.set("orthographique (p)", false));

	resetCameraButton.setup("reinitialiser la camera (c)");
	resetCameraButton.addListener(this, &Application::resetCamera);
	groupCamera.add(&resetCameraButton);

	frameCameraButton.setup("centrer sur l'objet (f)");
	frameCameraButton.addListener(this, &Application::frameCameraOnObject);
	groupCamera.add(&frameCameraButton);

	showAllButton.setup("afficher toute la scene (0)");
	showAllButton.addListener(this, &Application::showWholeScene);
	groupCamera.add(&showAllButton);

	gui.add(&groupCamera);
}

void Application::setupDepthOfFieldGui() {
	groupDepthOfField.setup("profondeur de champ");
	groupDepthOfField.add(depthOfFieldEnabled.set("activer", false));
	groupDepthOfField.add(focusDistance.set("distance nette", activeCamera().getRadius(), 1.0f, 5000.0f));
	groupDepthOfField.add(focusRange.set("zone nette", 200.0f, 10.0f, 2000.0f));
	groupDepthOfField.add(maxBlur.set("flou maximal (px)", 8.0f, 0.0f, 20.0f));
	gui.add(&groupDepthOfField);
}

void Application::resetCamera() {
	cameraFov = SceneCamera::DEFAULT_FOV;
	cameraNear = SceneCamera::DEFAULT_NEAR;
	cameraFar = SceneCamera::DEFAULT_FAR;
	cameraOrtho = false;
	activeCamera().reset();
	frameCameraOnObject();
}

void Application::selectCamera(size_t index) {
	if (index >= cameraCount) return;

	activeCameraIndex = index;
	cameraController.setCamera(activeCamera());
	loadCameraAttributesToGui();
	frameCameraOnObject();
	syncModelCameras();

	ofLog() << "<camera: " << activeCamera().getName() << ">";
}

void Application::onCameraSelectorChanged(int & value) {
	size_t index = static_cast<size_t>(value - 1);
	if (index != activeCameraIndex) selectCamera(index);
}

void Application::loadCameraAttributesToGui() {
	const SceneCamera & camera = activeCamera();
	cameraFov = camera.getFov();
	cameraNear = camera.getNearClip();
	cameraFar = camera.getFarClip();
	cameraOrtho = camera.isOrtho();
	cameraNameLabel = camera.getName();

	int selectorValue = static_cast<int>(activeCameraIndex) + 1;
	if (cameraSelector.get() != selectorValue) cameraSelector = selectorValue;
}

void Application::frameCameraOnObject() {
	auto centerOf = [](const SceneObject3d & model) { return model.getCenter3d(); };

	glm::vec3 sum(0.f);
	int count = 0;
	float largest = 0.f;
	auto accumulate = [&](const unordered_set<SceneObject *> & objects) {
		for (SceneObject * object : objects) {
			if (auto * model = dynamic_cast<SceneObject3d *>(object)) {
				sum += centerOf(*model);
				largest = max(largest, max(model->size.x, model->size.y));
				++count;
			}
		}
	};

	if (scene.isIsolating()) {
		accumulate(scene.getIsolated());
		if (count > 0) {
			activeCamera().frame(sum / static_cast<float>(count), largest);
			return;
		}
	}

	accumulate(transformTool.getSelection());
	if (count == 0) {
		for (const auto & object : scene.getObjects()) {
			if (auto * model = dynamic_cast<SceneObject3d *>(object.get())) {
				sum += centerOf(*model);
				++count;
			}
		}
	}
	if (count == 0) return;

	activeCamera().setTarget(sum / static_cast<float>(count));
}

bool Application::isolateSelection() {
	unordered_set<SceneObject *> models;
	for (SceneObject * object : transformTool.getSelection()) {
		if (dynamic_cast<SceneObject3d *>(object)) models.insert(object);
	}
	if (models.empty()) return false;
	if (models == scene.getIsolated()) return true;

	scene.isolate(models);
	ofLog() << "<isolation: " << models.size() << " modele(s)>";
	return true;
}

void Application::showWholeScene() {
	if (!scene.isIsolating()) return;
	scene.clearIsolation();
	frameCameraOnObject();
	ofLog() << "<isolation terminee>";
}

void Application::syncModelCameras() {
	for (const auto & object : scene.getObjects()) {
		if (auto * model = dynamic_cast<SceneObject3d *>(object.get())) {
			model->camera = &activeCamera();
		}
	}
}

void Application::drawCameraLabel() const {
	const SceneCamera & camera = activeCamera();
	string text = "camera " + ofToString(activeCameraIndex + 1) + "/" + ofToString(cameraCount)
		+ " : " + camera.getName() + (camera.isOrtho() ? " (orthogonale)" : " (perspective)");
	if (scene.isIsolating()) {
		const auto & isolated = scene.getIsolated();
		text += isolated.size() == 1 ? " | isole : " + (*isolated.begin())->name + " (0 = tout)"
									 : " | isole : " + ofToString(isolated.size()) + " objets (0 = tout)";
	}
	ofDrawBitmapStringHighlight(text, ofGetWidth() - 8 * static_cast<int>(text.size()) - 20, 24,
		ofColor(0, 0, 0, 160), ofColor(255));
}

bool Application::isOverUi(int x, int y) const {
	if (!showGui) return collapsedGuiButton.inside(x, y);
	return x< canvasLeft() || x >= canvasRight();
}

float Application::canvasLeft() const {
	return showGui ? (gui.getWidth() + scrollbarTrackWidth) : 0.0f;
}

float Application::canvasRight() const {
	return showGui ? ofGetWidth() - inspector.getWidth() : ofGetWidth();
}

void Application::exit() {
	importButton.removeListener(this, &Application::onImportPressed);
	recordButton.removeListener(this, &Application::onRecordPressed);
	resetButton.removeListener(this, &Application::onResetPressed);
	histogramButton.removeListener(this, &Application::onHistogramPressed);
	buttonUndo.removeListener(this, &Application::buttonUndoPressed);
	buttonRedo.removeListener(this, &Application::buttonRedoPressed);
	variantTintColor.removeListener(this, &Application::onColorChanged);

	backgroundColor.removeListener(this, &Application::onColorChanged);
	strokeColor.removeListener(this, &Application::onColorChanged);
	fillColor.removeListener(this, &Application::onColorChanged);
	importModelButton.removeListener(this, &Application::onImportModelPressed);
	cameraSelector.removeListener(this, &Application::onCameraSelectorChanged);
	frameCameraButton.removeListener(this, &Application::frameCameraOnObject);
	showAllButton.removeListener(this, &Application::showWholeScene);

	for (auto * parameter : transformParameters()) {
		parameter->removeListener(this, &Application::onTransformChanged);
	}

	for (auto * parameter : transformParameters()) {
		parameter->removeListener(this, &Application::onTransformChanged);
	}

	cursor.exit();

	ofLog() << "<app::exit>";
}

array<ofParameter<float> *, 5> Application::transformParameters() {
	return { &transformX, &transformY, &transformRotation, &transformWidth, &transformHeight };

}
