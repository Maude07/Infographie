#pragma once
#include "ofMain.h"
#include "sceneLight.h"

class LightRig {
public:
	vector<SceneLight> lights;

	void setupDefaults(const glm::vec3 & center);

	void enableAll() const;
	void disableAll() const;
};
