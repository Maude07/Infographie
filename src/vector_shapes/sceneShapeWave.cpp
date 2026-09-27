#include "sceneShapeWave.h"

void SceneShapeWave::draw() const {
	applyStyle();

	float totalWidth = size.x;
	float waveHeight = size.y;
	int resolution = 50; 
	float frequency = 3.0f; 

	if (filled) {
		ofFill();
	} else {
		ofNoFill();
	}

	ofBeginShape();

	if (filled) {
		ofVertex(position.x - totalWidth / 2.0f, position.y + waveHeight / 2.0f);
	}

	for (int i = 0; i <= resolution; i++) {
		float t = (float)i / resolution;
		float x = position.x - totalWidth / 2.0f + t * totalWidth;

		float y = position.y + sin(t * frequency * TWO_PI) * (waveHeight * 0.5f);

		ofVertex(x, y);
	}

	if (filled) {
		ofVertex(position.x + totalWidth / 2.0f, position.y + waveHeight / 2.0f);
		ofVertex(position.x - totalWidth / 2.0f, position.y + waveHeight / 2.0f);
	}

	ofEndShape(true);
}




ofRectangle SceneShapeWave::getBounds() const {
	return ofRectangle(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
}

bool SceneShapeWave::contains(float x, float y) const {
	float rx = size.x / 2.0f;
	float ry = size.y / 2.0f;
	if (rx <= 0.0f || ry <= 0.0f) return false;

	float dx = (x - position.x) / rx;
	float dy = (y - position.y) / ry;
	return (dx * dx + dy * dy) <= 1.0f;
}
