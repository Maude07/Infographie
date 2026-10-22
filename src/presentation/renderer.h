#pragma once

#include "ofMain.h"
#include "domain/scene/scene.h"
#include "domain/camera/sceneCamera.h"

class Renderer {
public:

	ofColor backgroundColor = ofColor(31);
	ofColor strokeColor = ofColor(255);
	float strokeWeight = 1.0f;
	string text;
	float visibleOffsetX = 0.0f;
	float visibleRightX = 0.0f;

	void setup();
	void update();
	void draw(const Scene & scene, SceneCamera & camera) const;

private:

	static constexpr int fontSize = 64;
	static constexpr float underlineOffset = fontSize / 2.0f;

	ofTrueTypeFont font;
	ofRectangle textBounds;
};
