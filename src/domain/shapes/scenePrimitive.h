#pragma once
#include "domain/sceneObject.h"

class ScenePrimitive : public SceneObject {
public:
	ofColor fillColor = ofColor::white;
	ofColor lineColor = ofColor::black;
	float lineWidth = 1.0f;
	bool filled = true;

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
