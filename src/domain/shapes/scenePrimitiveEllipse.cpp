#include "domain/shapes/scenePrimitiveEllipse.h"

void ScenePrimitiveEllipse::draw() const {
	ofPushStyle();
	applyStyle();
	ofDrawEllipse(position.x, position.y, size.x, size.y);
	if (filled) {
		ofNoFill();
		ofSetColor(lineColor);
		ofDrawEllipse(position.x, position.y, size.x, size.y);
	}
	ofPopStyle();
}

ofRectangle ScenePrimitiveEllipse::getBounds() const {
	return ofRectangle(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
}

void ScenePrimitiveEllipse::setBounds(const ofRectangle & bounds) {
	position = { bounds.getCenter().x, bounds.getCenter().y };
	size = { bounds.width, bounds.height };
}

bool ScenePrimitiveEllipse::contains(float x, float y) const {
	float rx = size.x / 2.0f;
	float ry = size.y / 2.0f;
	if (rx <= 0.0f || ry <= 0.0f) return false;

	float dx = (x - position.x) / rx;
	float dy = (y - position.y) / ry;
	return (dx * dx + dy * dy) <= 1.0f;
}
