#include "infrastructure/renderer.h"

void Renderer::setup() {
	ofSetFrameRate(60);
	ofSetBackgroundColor(31);
	font.load("consolas.ttf", fontSize);
}

void Renderer::update() {
	textBounds = font.getStringBoundingBox(text, 0, 0);
}

void Renderer::draw(const Scene & scene) const {
	ofClear(backgroundColor);
	scene.draw();

	float centerX = visibleOffsetX + (ofGetWidth() - visibleOffsetX) / 2.0f;
	float left = centerX - textBounds.getWidth() / 2.0f;
	float right = centerX + textBounds.getWidth() / 2.0f;
	float baseline = ofGetHeight() / 2.0f + textBounds.getHeight() / 2.0f;

	ofPushStyle();
	ofSetColor(strokeColor);
	ofSetLineWidth(strokeWeight);
	font.drawString(text, left, baseline);
	ofDrawLine(left, baseline + underlineOffset, right, baseline + underlineOffset);
	ofPopStyle();
};
