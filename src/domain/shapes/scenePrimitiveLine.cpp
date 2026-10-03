#include "domain/shapes/scenePrimitiveLine.h"

void ScenePrimitiveLine::draw() const {
	ofPushStyle();
	applyStyle();
	ofDrawLine(position.x, position.y, size.x, size.y);
	ofPopStyle();
}

ofRectangle ScenePrimitiveLine::getBounds() const {
	float minX = std::min(position.x, size.x);
	float minY = std::min(position.y, size.y);
	float maxX = std::max(position.x, size.x);
	float maxY = std::max(position.y, size.y);
	return ofRectangle(minX, minY, maxX - minX, maxY - minY);
}

void ScenePrimitiveLine::setBounds(const ofRectangle & bounds) {
	ofRectangle old = getBounds();
	auto remap = [&](const glm::vec2 & p) {
		float tx = old.width > 0.0f ? (p.x - old.x) / old.width : 0.0f;
		float ty = old.height > 0.0f ? (p.y - old.y) / old.height : 0.0f;
		return glm::vec2(bounds.x + tx * bounds.width, bounds.y + ty * bounds.height);
	};
	glm::vec2 start = remap(position);
	glm::vec2 end = remap(size);
	position = start;
	size = end;
}


bool ScenePrimitiveLine::contains(float x, float y) const {
	glm::vec2 p(x, y);
	glm::vec2 a = position;
	glm::vec2 b = size;
	glm::vec2 ab = b - a;

	float lengthSquared = glm::dot(ab, ab);
	float t = lengthSquared > 0.0f ? glm::clamp(glm::dot(p - a, ab) / lengthSquared, 0.0f, 1.0f) : 0.0f;
	glm::vec2 closest = a + t * ab;
	float tolerance = std::max(lineWidth, 4.0f);
	return glm::distance(p, closest) <= tolerance;
}
