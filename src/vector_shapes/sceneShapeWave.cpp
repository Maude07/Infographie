#include "vector_shapes/sceneShapeWave.h"

void SceneShapeWave::draw() const {
	ofPushStyle();
	applyStyle();

	float totalWidth = size.x;
	float waveHeight = size.y;
	int resolution = 50;
	float frequency = 3.0f;

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
	ofPopStyle();
}

ofRectangle SceneShapeWave::getBounds() const {
	return ofRectangle(position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x, size.y);
}

void SceneShapeWave::setBounds(const ofRectangle & bounds) {
	position = { bounds.getCenter().x, bounds.getCenter().y };
	size = { bounds.width, bounds.height };
}

bool SceneShapeWave::contains(float x, float y) const {
	return getBounds().inside(x, y);
}
