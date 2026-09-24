// IFT3100A25_Interface/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup() {
	ofSetFrameRate(60);
	ofSetBackgroundColor(31);

	fontSize = 64;

	lineOffset = fontSize / 2.0f;

	font.load("consolas.ttf", fontSize);
}

void Renderer::update() {
	ofSetColor(strokeColor);
	ofSetLineWidth(strokeWeight);

	boundingBox = font.getStringBoundingBox(text, 0, 0);
}

void Renderer::draw(const Scene & scene) {
	ofClear(backgroundColor);

	scene.draw();

	font.drawString(
		text,
		(ofGetWidth() / 2.0f) - (boundingBox.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (boundingBox.getHeight() / 2.0f));

	ofDrawLine(
		(ofGetWidth() / 2.0f) - (boundingBox.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (boundingBox.getHeight() / 2.0f) + lineOffset,
		(ofGetWidth() / 2.0f) + (boundingBox.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (boundingBox.getHeight() / 2.0f) + lineOffset);
};
