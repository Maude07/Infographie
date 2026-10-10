#pragma once

#include "ofMain.h"
#include "domain/scene/scene.h"

class Renderer {
public:

	ofColor backgroundColor = ofColor(31);
	ofColor strokeColor = ofColor(255);
	float strokeWeight = 1.0f;
	string text;
	float visibleOffsetX = 0.0f;

	void setup();
	void update();
	void draw(const Scene & scene) const;

private:

	static constexpr int fontSize = 64;
	static constexpr float underlineOffset = fontSize / 2.0f;

	ofTrueTypeFont font;
	ofRectangle textBounds;
};
