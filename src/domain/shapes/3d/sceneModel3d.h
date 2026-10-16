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
		SceneObject::setBounds(ofRectangle(b.x, b.y, side, side));
	}

	ofRectangle getScreenBounds() const override;
	glm::vec2 toLocal(const glm::vec2 & screen) const override;

	glm::vec3 worldCenter() const;
	float worldBoundingRadius() const;

	float rotationY = 0.0f;
	float depth = 0.0f;
	const SceneCamera * camera = nullptr; // camera qui dessine ce modele

private:
	string modelId;
	shared_ptr<ofxAssimpModelLoader> model;
};
