#pragma once
#include "ofMain.h"

using namespace std;

enum class LightType {
	Ambient,
	Directional,
	Point,
	Spot
};

class SceneLight {
public:
	static constexpr int typeCount = 4;
	static string typeLabel(LightType type);

	string name;
	LightType type = LightType::Point;
	bool enabled = true;

	ofColor color = ofColor::white;
	glm::vec3 position { 0, 0, 300 };
	glm::vec3 target { 0, 0, 0 };
	float constant = 1.f, linear = 0.f, quadratic = 0.f;
	float spotAngle = 30.f, spotConcentration = 2.f;

	void apply() const;
	void enable() const;
	void disable() const;

private:
	mutable ofLight light;
};
