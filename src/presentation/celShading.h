#pragma once

#include "ofMain.h"

class LightRig;

namespace celShading {
	ofShader * shader();
	void setLightRig(const LightRig * rig);
	glm::vec3 lightWorldPosition(const glm::vec3 & fallback);
}