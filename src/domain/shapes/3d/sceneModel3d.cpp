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

void SceneModel3d::draw() const {
	if (!model) return;

	const glm::vec3 dims = model->getSceneMax() - model->getSceneMin();
	const float largest = max({ dims.x, dims.y, dims.z });
	if (largest <= 0.0f) return;

	const glm::vec3 c = model->getSceneCenter();
	const glm::vec2 center = position + size * 0.5f;
	const float s = min(size.x, size.y) / largest;

	ofPushStyle();
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
	ofPopStyle();
}
