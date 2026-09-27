#include "sceneShapeFunkyCircles.h"

void SceneShapeFunkyCircles::draw() const {
	applyStyle();
	ofDrawEllipse(position.x, position.y, size.x, size.y);
	if (filled) {
		ofNoFill();
		ofSetColor(lineColor);
		
		ofDrawEllipse(position.x, position.y, size.x, size.y);
		for (int i = 0; i < 7; i++) {
			float offsetX = ofRandom(-size.x / 4.0f, size.x / 4.0f);
			float offsetY = ofRandom(-size.y / 4.0f, size.y / 4.0f);
			float innerWidth = ofRandom(size.x / 8.0f, size.x / 2.0f);
			float innerHeight = ofRandom(size.y / 8.0f, size.y / 2.0f);
			ofDrawEllipse(position.x + offsetX, position.y + offsetY, innerWidth, innerHeight);
		}


	}
}

ofRectangle SceneShapeFunkyCircles::getBounds() const {
	return ofRectangle(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
}

bool SceneShapeFunkyCircles::contains(float x, float y) const {
	float rx = size.x / 2.0f;
	float ry = size.y / 2.0f;
	if (rx <= 0.0f || ry <= 0.0f) return false;

	float dx = (x - position.x) / rx;
	float dy = (y - position.y) / ry;
	return (dx * dx + dy * dy) <= 1.0f;
}
