#include "scenePrimitiveLine.h"

void ScenePrimitiveLine::draw() const {
	applyStyle();
	ofDrawLine(position.x, position.y, size.x, size.y);
}

ofRectangle ScenePrimitiveLine::getBounds() const {
	float minX = std::min(position.x, size.x);
	float minY = std::min(position.y, size.y);
	float maxX = std::max(position.x, size.x);
	float maxY = std::max(position.y, size.y);
	return ofRectangle(minX, minY, maxX - minX, maxY - minY);
}

bool ScenePrimitiveLine::contains(float x, float y) const {
	glm::vec2 p(x, y);
	glm::vec2 a = position;
	glm::vec2 b = size;
	glm::vec2 ab = b - a;

	float t = glm::clamp(glm::dot(p - a, ab) / glm::dot(ab, ab), 0.0f, 1.0f);
	glm::vec2 closest = a + t * ab;
	float tolerance = std::max(lineWidth, 4.0f);
	return glm::distance(p, closest) <= tolerance;
}
