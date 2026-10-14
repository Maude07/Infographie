#include "sceneModel3d.h"

SceneModel3d::SceneModel3d(const string& id,
	shared_ptr<ofxAssimpModelLoader> m,
	const glm::vec2& center,
	float footprint)
	: modelId(id)
	, model(move(m)) {
	name = modelId;
	size = { footprint, footprint };
	position = center - size * 0.5f;
}

ofRectangle SceneModel3d::getScreenBounds() const {
	if (!camera) return getBounds();

	const glm::vec2 c2 = position + size * 0.5f;
	const float h = min(size.x, size.y) * 0.5f;
	ofRectangle r;
	bool first = true;
	for (int i = 0; i < 8; ++i) {
		glm::vec3 corner(
			c2.x + ((i & 1) ? h : -h),
			c2.y + ((i & 2) ? h : -h),
			depth + ((i & 4) ? h : -h));
		glm::vec3 s = camera->worldToScreen(corner);
		if (first) {
			r.set(s.x, s.y, 0, 0);
			first = false;
		} else {
			r.growToInclude(s.x, s.y);
		}
	}
	return r;
}

glm::vec2 SceneModel3d::toLocal(const glm::vec2 & screen) const {
	if (!camera) return screen;
	return camera->screenToPlane(screen, depth);
}

void SceneModel3d::draw() const {
	if (!model) return;

	const glm::vec3 dims = model->getSceneMax() - model->getSceneMin();
	const float largest = max({ dims.x, dims.y, dims.z });
	if (largest <= 0.0f) return;

	const glm::vec3 c = model->getSceneCenter();
	const glm::vec2 center = position + size * 0.5f;
	const float side = min(size.x, size.y);
	const float s = side / largest;

	static ofLight light;
	light.setPointLight();
	light.setDiffuseColor(ofColor(255));
	light.setPosition(center.x - side, center.y - side, side * 3.0f);

	ofPushStyle();
	ofEnableLighting();
	light.enable();

	ofPushMatrix();
	ofTranslate(center.x, center.y, depth);
	ofRotateYDeg(rotationY);
	ofScale(s, s, s);
	ofTranslate(c.x, c.y, -c.z);

	glClear(GL_DEPTH_BUFFER_BIT);
	ofEnableDepthTest();
	model->drawFaces();
	ofDisableDepthTest();
	ofPopMatrix();

	light.disable();
	ofDisableLighting();
	ofPopStyle();
}
