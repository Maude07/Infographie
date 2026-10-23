#pragma once

#include "ofMain.h"
#include "domain/scene/scene.h"
#include "domain/camera/sceneCamera.h"

class Renderer {
public:

	ofColor backgroundColor = ofColor(31);

	void setup();
	void draw(const Scene & scene, SceneCamera & camera) const;
};
