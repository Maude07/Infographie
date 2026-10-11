#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "../../scene/sceneObject.h"

class SceneModel3d : public SceneObject {
public:
	SceneModel3d(const string & modelId,
		shared_ptr<ofxAssimpModelLoader> model,
		const glm::vec2 & center,
		float footprint = 150.0f);

	void draw() const override;

	void setBounds(const ofRectangle& b) override {
		float side = max(b.width, b.height);
		SceneObject::setBounds(ofRectangle(b.x, b.y, side, side));
	}

	float rotationY = 0.0f;
	float depth = 0.0f;
	bool bShowBoundingBox = false;

private:
	string modelId;
	shared_ptr<ofxAssimpModelLoader> model;
};
