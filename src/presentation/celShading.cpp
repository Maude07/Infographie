#include "presentation/celShading.h"
#include "domain/lighting/lightRig.h"
#include "domain/lighting/sceneLight.h"

namespace {

	const LightRig * activeLightRig = nullptr;

} // namespace

namespace celShading {

	void setLightRig(const LightRig * rig) {
		activeLightRig = rig;
	}

	glm::vec3 lightWorldPosition(const glm::vec3 & fallback) {
		if (!activeLightRig) return fallback;

		for (const SceneLight & light : activeLightRig->lights) {
			if (!light.enabled) continue;
			switch (light.type) {
			case LightType::Directional: {
				glm::vec3 dir = light.target - light.position;
				if (glm::dot(dir, dir) < 1e-6f) dir = glm::vec3(0, 0, -1);
				else dir = glm::normalize(dir);
				return fallback - dir * 2000.0f;
			}
			case LightType::Point:
			case LightType::Spot:
				return light.position;
			case LightType::Ambient:
				break;
			}
		}
		return fallback;
	}

	ofShader * shader() {
		static ofShader program;
		static bool attempted = false;
		if (!attempted) {
			attempted = true;
			if (!program.load("shaders/cel.vert", "shaders/cel.frag")) {
				ofLogError("celShading") << "echec du chargement de bin/data/shaders/cel.vert et cel.frag";
			}
		}
		return program.isLoaded() ? &program : nullptr;
	}

}