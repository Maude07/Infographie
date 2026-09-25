#include "scenePrimitiveEllipse.h"

void ScenePrimitiveEllipse::draw() const {
	applyStyle();
	ofDrawEllipse(position.x, position.y, size.x, size.y);
	if (filled) {
		ofNoFill();
		ofSetColor(lineColor);
		ofDrawEllipse(position.x, position.y, size.x, size.y);
	}
}

ofRectangle ScenePrimitiveEllipse::getBounds() const {
	return ofRectangle(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
}

bool ScenePrimitiveEllipse::contains(float x, float y) const {
	float rx = size.x / 2.0f;
	float ry = size.y / 2.0f;
	if (rx <= 0.0f || ry <= 0.0f) return false;

	float dx = (x - position.x) / rx;
	float dy = (y - position.y) / ry;
	return (dx * dx + dy * dy) <= 1.0f;
}
