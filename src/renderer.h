// IFT3100A25_Interface/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"
#include "scene.h"

class Renderer {
public:
	ofTrueTypeFont font;

	ofColor backgroundColor;

	ofColor strokeColor;

	ofRectangle boundingBox;

	string text;

	float strokeWeight;

	float lineOffset;

	int fontSize;

	void setup();
	void update();
	void draw(const Scene & scene);
};
