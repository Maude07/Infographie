#include "sceneShapeStars.h"

void SceneShapeStars::draw() const {
	applyStyle();

	int numPoints = 8; 
	float outerRadius = glm::length(size) * 0.5f; 
	float innerRadius = outerRadius * 0.4f; 

	ofBeginShape();
	float angleStep = TWO_PI / (numPoints * 2);

	for (int i = 0; i < numPoints * 2; i++) {
		float radius = (i % 2 == 0) ? outerRadius : innerRadius;
		float angle = i * angleStep - HALF_PI;

		float x = position.x + cos(angle) * radius;
		float y = position.y + sin(angle) * radius;

		ofVertex(x, y);
	}
	ofEndShape(true); 
}

ofRectangle SceneShapeStars::getBounds() const {
	return ofRectangle(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
}

bool SceneShapeStars::contains(float x, float y) const {
	float rx = size.x / 2.0f;
	float ry = size.y / 2.0f;
	if (rx <= 0.0f || ry <= 0.0f) return false;

	float dx = (x - position.x) / rx;
	float dy = (y - position.y) / ry;
	return (dx * dx + dy * dy) <= 1.0f;
}
