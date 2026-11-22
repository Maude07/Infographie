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

	void setLightUniforms(ofShader & program, const glm::vec3 & fallbackLightWorld) {
		const glm::mat4 view = ofGetCurrentViewMatrix();
		const glm::mat3 viewRotation(view);
		auto toView = [&](const glm::vec3 & p) { return glm::vec3(view * glm::vec4(p, 1.0f)); };

		vector<int> types(maxLights, 0);
		vector<glm::vec3> positions(maxLights, glm::vec3(0.0f));
		vector<glm::vec3> directions(maxLights, glm::vec3(0.0f, 0.0f, -1.0f));
		vector<glm::vec3> colors(maxLights, glm::vec3(0.0f));
		vector<glm::vec3> attenuations(maxLights, glm::vec3(1.0f, 0.0f, 0.0f));
		vector<float> spotCosCutoffs(maxLights, 0.0f);
		vector<float> spotExponents(maxLights, 0.0f);
		int count = 0;

		if (activeLightRig) {
			for (const SceneLight & light : activeLightRig->lights) {
				if (!light.enabled) continue;
				if (count >= maxLights) break;

				glm::vec3 dir = light.target - light.position;
				dir = glm::dot(dir, dir) < 1e-6f ? glm::vec3(0, 0, -1) : glm::normalize(dir);

				const ofFloatColor c(light.color);
				types[count] = static_cast<int>(light.type);
				positions[count] = toView(light.position);
				directions[count] = glm::normalize(viewRotation * dir);
				colors[count] = glm::vec3(c.r, c.g, c.b);
				attenuations[count] = glm::vec3(light.constant, light.linear, light.quadratic);
				spotCosCutoffs[count] = std::cos(glm::radians(light.spotAngle));
				spotExponents[count] = light.spotConcentration;
				++count;
			}
		} else {
			types[0] = static_cast<int>(LightType::Point);
			positions[0] = toView(fallbackLightWorld);
			colors[0] = glm::vec3(1.0f);
			count = 1;
		}

		program.setUniform1i("lightCount", count);
		program.setUniform1iv("lightType", types.data(), maxLights);
		program.setUniform3fv("lightPosition", &positions[0].x, maxLights);
		program.setUniform3fv("lightDirection", &directions[0].x, maxLights);
		program.setUniform3fv("lightColor", &colors[0].x, maxLights);
		program.setUniform3fv("lightAttenuation", &attenuations[0].x, maxLights);
		program.setUniform1fv("lightSpotCosCutoff", spotCosCutoffs.data(), maxLights);
		program.setUniform1fv("lightSpotExponent", spotExponents.data(), maxLights);
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