#include "vector_shapes/sceneShapeStars.h"

namespace {
	constexpr int kPointCount = 8;
	constexpr float kHalf = 0.5f;
	constexpr float kInnerRadiusScale = 0.4f;
}

void SceneShapeStars::draw() const {
	ofPushStyle();
	applyStyle();

	float radiusX = size.x * kHalf;
	float radiusY = size.y * kHalf;

	ofBeginShape();
	float angleStep = TWO_PI / (kPointCount * 2);

	for (int i = 0; i < kPointCount * 2; i++) {
		float scale = (i % 2 == 0) ? 1.0f : kInnerRadiusScale;
		float angle = i * angleStep - HALF_PI;
		float x = position.x + cos(angle) * radiusX * scale;
		float y = position.y + sin(angle) * radiusY * scale;
		ofVertex(x, y);
	}
	ofEndShape(true);
	ofPopStyle();
}

ofRectangle SceneShapeStars::getBounds() const {
	return ofRectangle(position.x - size.x * kHalf, position.y - size.y * kHalf, size.x, size.y);
}

void SceneShapeStars::setBounds(const ofRectangle & bounds) {
	position = { bounds.getCenter().x, bounds.getCenter().y };
	size = { bounds.width, bounds.height };
}

bool SceneShapeStars::contains(float x, float y) const {
	float rx = size.x * kHalf;
	float ry = size.y * kHalf;
	if (rx <= 0.0f || ry <= 0.0f) return false;

	float dx = (x - position.x) / rx;
	float dy = (y - position.y) / ry;
	return (dx * dx + dy * dy) <= 1.0f;
}
