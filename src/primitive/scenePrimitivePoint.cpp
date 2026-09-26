#include "scenePrimitivePoint.h"

void ScenePrimitivePoint::draw() const {
	ofFill();
	ofSetColor(lineColor);
	ofDrawCircle(position.x, position.y, lineWidth);
}

ofRectangle ScenePrimitivePoint::getBounds() const {
	return ofRectangle(position.x - lineWidth, position.y - lineWidth, lineWidth * 2.0f, lineWidth * 2.0f);
}

bool ScenePrimitivePoint::contains(float x, float y) const {
	return glm::distance(glm::vec2(x, y), position) <= lineWidth;
}
