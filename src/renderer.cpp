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

	float visibleWidth = ofGetWidth() - visibleOffsetX;
	float centerX = visibleOffsetX + (visibleWidth / 2.0f);

	font.drawString(
		text,
		centerX - (boundingBox.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (boundingBox.getHeight() / 2.0f));

	ofDrawLine(
		centerX - (boundingBox.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (boundingBox.getHeight() / 2.0f) + lineOffset,
		centerX + (boundingBox.getWidth() / 2.0f),
		(ofGetHeight() / 2.0f) + (boundingBox.getHeight() / 2.0f) + lineOffset);
};
