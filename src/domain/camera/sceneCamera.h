#pragma once
#include "ofMain.h"

class SceneCamera {
public:
	SceneCamera();
	SceneCamera(const std::string & name, float presetLongitude, float presetLatitude);

	void begin() { camera.begin(); }
	void end() { camera.end(); }

	void orbit(float dLongitude, float dLatitude);
	void zoom(float factor);
	void pan(float dx, float dy);
	void reset();
	void setTarget(const glm::vec3 & newTarget);
	void frame(const glm::vec3 & newTarget, float objectSize);

	void setFov(float fovDeg);
	void setClip(float nearClip, float farClip);
	void setOrtho(bool enabled);

	static constexpr float DEFAULT_FOV = 60.f;
	static constexpr float DEFAULT_NEAR = 1.f;
	static constexpr float DEFAULT_FAR = 10000.f;
	float getFov() const { return camera.getFov(); }
	float getRadius() const { return radius; }
	bool isOrtho() const { return camera.getOrtho(); }
	float getNearClip() const { return camera.getNearClip(); }
	float getFarClip() const { return camera.getFarClip(); }
	const std::string & getName() const { return name; }
	glm::vec3 getTarget() const { return target; }

	glm::vec3 worldToScreen(const glm::vec3 & world) const { return camera.worldToScreen(world); }
	glm::vec2 screenToPlane(const glm::vec2 & screen, float planeZ) const;

private:
	void apply();

	ofCamera camera;
	std::string name = "face";
	float presetLongitude = 0, presetLatitude = 0;
	float longitude = 0, latitude = 0, radius = 10;
	float baseRadius = 10;
	glm::vec3 target { 0, 0, 0 };
};

