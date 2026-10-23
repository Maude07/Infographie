#include "presentation/renderer.h"

void Renderer::setup() {
	ofSetFrameRate(60);
	ofSetBackgroundColor(31);
}

void Renderer::draw(const Scene & scene, SceneCamera & camera) const {
	ofClear(backgroundColor);
	scene.draw(&camera);
}
