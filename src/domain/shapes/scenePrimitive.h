#pragma once
#include "ofMain.h"
#include "domain/sceneEntity.h"
#include "domain/sceneObject.h"

class ScenePrimitive : public SceneObject, SceneEntity {
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
