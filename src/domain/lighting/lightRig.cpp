#include "lightRig.h"

void LightRig::setupDefaults(const glm::vec3 & center) {
	lights.clear();
	lights.reserve(4);

	SceneLight ambient;
	ambient.name = "ambiante";
	ambient.type = LightType::Ambient;
	ambient.color = ofColor(60, 60, 70);
	ambient.position = center;
	lights.push_back(ambient);

	SceneLight sun;
	sun.name = "soleil";
	sun.type = LightType::Directional;
	sun.color = ofColor(255, 244, 214);
	sun.position = center + glm::vec3(-400, -500, 600);
	sun.target = center;
	lights.push_back(sun);

	SceneLight point;
	point.name = "ampoule";
	point.type = LightType::Point;
	point.color = ofColor(255, 140, 60);
	point.position = center + glm::vec3(300, -200, 250);
	point.linear = 0.002f;
	lights.push_back(point);

	SceneLight spot;
	spot.name = "projecteur";
	spot.type = LightType::Spot;
	spot.color = ofColor(80, 160, 255);
	spot.position = center + glm::vec3(0, -400, 400);
	spot.target = center;
	spot.spotAngle = 25.f;
	spot.spotConcentration = 10.f;
	lights.push_back(spot);
}

void LightRig::enableAll() const {
	for (const SceneLight & light : lights) {
		if (light.enabled) {
			light.apply();
			light.enable();
		} else {
			light.disable();
		}
	}
}

void LightRig::disableAll() const {
	for (const SceneLight & light : lights) light.disable();
}
