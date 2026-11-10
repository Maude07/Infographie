#pragma once

#include "ofMain.h"
#include "domain/scene/scene.h"
#include "domain/camera/sceneCamera.h"
#include "infrastructure/skyboxCatalog.h"

struct SkyboxViewOptions {
	bool enabled = false;
	int activeIndex = 0;
};

class Renderer {
public:

	ofColor backgroundColor = ofColor(31);

	void setup();
	void draw(const Scene & scene, SceneCamera & camera, const SkyboxViewOptions & skybox = {}) const;

	const SkyboxCatalog & getSkyboxCatalog() const { return skyboxCatalog; }

private:
	mutable SkyboxCatalog skyboxCatalog;
};
