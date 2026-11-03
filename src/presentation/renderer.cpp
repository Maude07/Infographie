#include "presentation/renderer.h"

#include <functional>

void Renderer::setup() {
	ofSetFrameRate(60);
	ofSetBackgroundColor(31);
	if (!ofIsGLProgrammableRenderer()) {
		ofLogError("Renderer") << "skybox requires OpenGL 3.2+ (setGLVersion in main.cpp)";
		return;
	}
	skyboxCatalog.loadFromFolder("skybox");
}

void Renderer::draw(const Scene & scene, SceneCamera & camera, const SkyboxViewOptions & skybox) const {
	ofClear(backgroundColor);

	std::function<void()> drawSkybox;
	if (skybox.enabled && skyboxCatalog.hasAnyLoaded()) {
		const int index = ofClamp(
			skybox.activeIndex, 0, static_cast<int>(skyboxCatalog.size()) - 1);
		drawSkybox = [this, index]() { skyboxCatalog.draw(index); };
	}

	scene.draw(&camera, drawSkybox);
}
