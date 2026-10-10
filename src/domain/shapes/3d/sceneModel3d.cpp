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

	const glm::vec2 center = position + size * 0.5f;
	const float s = min(size.x, size.y);

	model->setPosition(center.x, center.y, depth);
	model->setScale(s, s, s);
	model->setRotation(0, rotationY, 0, 1, 0);
	model->setRotation(1, 180, 1, 0, 0);

	ofEnableDepthTest();
	model->drawFaces();
	ofDisableDepthTest();
}
