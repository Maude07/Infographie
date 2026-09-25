#include "scenePrimitivePoint.h"

void ScenePrimitivePoint::draw() const {
	applyStyle();
	ofDrawCircle(position.x, position.y, radius);
}

ofRectangle ScenePrimitivePoint::getBounds() const {
	return ofRectangle(position.x - radius, position.y - radius, radius * 2.0f, radius * 2.0f);
}

bool ScenePrimitivePoint::contains(float x, float y) const {
	return glm::distance(glm::vec2(x, y), position) <= radius;
}
