#include "domain/shapes/scenePrimitivePoint.h"

void ScenePrimitivePoint::draw() const {
	ofPushStyle();
	ofFill();
	ofSetColor(lineColor);
	ofDrawCircle(position.x, position.y, lineWidth);
	ofPopStyle();
}

ofRectangle ScenePrimitivePoint::getBounds() const {
	return ofRectangle(position.x - radius(), position.y - radius(), radius() * 2.0f, radius() * 2.0f);
}

void ScenePrimitivePoint::setBounds(const ofRectangle & bounds) {
	position = { bounds.getCenter().x, bounds.getCenter().y };
}

bool ScenePrimitivePoint::contains(float x, float y) const {
	return glm::distance(glm::vec2(x, y), position) <= radius();
}
