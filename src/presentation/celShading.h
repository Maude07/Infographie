#pragma once

#include "ofMain.h"

class LightRig;

namespace celShading {
	constexpr int maxLights = 8;

	ofShader * shader();
	void setLightRig(const LightRig * rig);

	void setLightUniforms(ofShader & program, const glm::vec3 & fallbackLightWorld);
}