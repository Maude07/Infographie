#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "sceneObject3d.h"

class SceneModel3d : public SceneObject3d {
public:
	SceneModel3d(const string & modelId,
		shared_ptr<ofxAssimpModelLoader> model,
		const glm::vec2 & center,
		float footprint = 150.0f);

	void draw() const override;

	ofRectangle getScreenBounds() const override;

private:
	string modelId;
	shared_ptr<ofxAssimpModelLoader> model;
};
