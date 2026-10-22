#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "../../scene/sceneObject.h"
#include "../../camera/sceneCamera.h"

class SceneModel3d : public SceneObject {
public:
	SceneModel3d(const string & modelId,
		shared_ptr<ofxAssimpModelLoader> model,
		const glm::vec2 & center,
		float footprint = 150.0f);

	void draw() const override;

	void setBounds(const ofRectangle& b) override {
		float side = max(b.width, b.height);
		ofRectangle square;
		square.setFromCenter(b.getCenter(), side, side);
		SceneObject::setBounds(square);
	}

	bool isSquare() const override { return true; }
	ofRectangle getScreenBounds() const override;
	glm::vec2 toLocal(const glm::vec2 & screen) const override;

	float rotationY = 0.0f;
	float depth = 0.0f;
	const SceneCamera * camera = nullptr;

private:
	string modelId;
	shared_ptr<ofxAssimpModelLoader> model;
};
