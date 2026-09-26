#pragma once
#include "sceneObject.h"
#include "ofMain.h"

class ScenePrimitive : public SceneObject {
public:
	ofColor fillColor = ofColor::white;
	ofColor lineColor = ofColor::black;
	float lineWidth = 1.0f;
	bool filled = true;

	virtual ~ScenePrimitive() = default;

	void setFilled(bool f) { filled = f; }

	protected:
		void applyStyle() const {
			ofSetLineWidth(lineWidth);
			if (filled) {
				ofFill();
				ofSetColor(fillColor);
			} else {
				ofNoFill();
				ofSetColor(lineColor);
			}
	}
};
