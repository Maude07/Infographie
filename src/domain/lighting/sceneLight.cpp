#include "sceneLight.h"

string SceneLight::typeLabel(LightType type) {
	switch (type) {
	case LightType::Ambient: return "ambiante";
	case LightType::Directional: return "directionnelle";
	case LightType::Point: return "ponctuelle";
	case LightType::Spot: return "projecteur";
	}
	return "?";
}

void SceneLight::apply() const {
	switch (type) {
	case LightType::Ambient:
		light.setPointLight();
		light.setAmbientColor(color);
		light.setDiffuseColor(ofColor(0));
		light.setSpecularColor(ofColor(0));
		light.setAttenuation(1.f, 0.f, 0.f);
		light.setPosition(position);
		return;
	case LightType::Directional:
		light.setDirectional();
		break;
	case LightType::Point:
		light.setPointLight();
		break;
	case LightType::Spot:
		light.setSpotlight(spotAngle, spotConcentration);
		break;
	}

	light.setAmbientColor(ofColor(0));
	light.setDiffuseColor(color);
	light.setSpecularColor(color);
	light.setPosition(position);
	light.setAttenuation(constant, linear, quadratic);

	if (type == LightType::Directional || type == LightType::Spot) {
		glm::vec3 direction = target - position;
		if (glm::length(direction) < 0.001f) direction = { 0, 0, -1 };
		direction = glm::normalize(direction);
		glm::vec3 up = std::abs(direction.y) > 0.99f ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
		light.lookAt(position + direction, up);
	}
}

void SceneLight::enable() const {
	light.enable();
}

void SceneLight::disable() const {
	light.disable();
}
