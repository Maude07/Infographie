#pragma once
#include "../../scene/sceneObject.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "sceneObject3d.h"

struct ModelVariant {
	bool stretchVertical = false;
	float verticalFactor = 1.5f;

	bool stretchHorizontal = false;
	float horizontalFactor = 1.5f;

	bool tinted = false;
	ofColor tint = ofColor(255, 140, 90);

	bool metallic = false;
	float metalness = 0.7f;

	glm::vec3 scaleFactors() const {
		return {
			stretchHorizontal ? horizontalFactor : 1.0f,
			stretchVertical ? verticalFactor : 1.0f,
			1.0f
		};
	}

	bool overridesMaterial() const { return tinted || metallic; }
	bool isOriginal() const { return !stretchVertical && !stretchHorizontal && !tinted && !metallic; }
	string label() const;
};

class SceneModel3d : public SceneObject3d {
public:
	SceneModel3d(const string & modelId,
		shared_ptr<ofxAssimpModelLoader> model,
		const glm::vec2 & center,
		float footprint = 150.0f,
		const ModelVariant & variant = ModelVariant());

	void draw() const override;

	ofRectangle getScreenBounds() const override;

	glm::vec3 worldCenter() const;
	float worldBoundingRadius() const;

	ModelVariant variant;

private:
	string modelId;
	shared_ptr<ofxAssimpModelLoader> model;
};
