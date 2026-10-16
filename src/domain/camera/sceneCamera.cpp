#include "SceneCamera.h"
#include "sceneModel3d.h"

SceneCamera::SceneCamera() { reset(); }

void SceneCamera::reset() {
	camera.setVFlip(true);
	camera.setFov(DEFAULT_FOV);
	camera.setNearClip(DEFAULT_NEAR);
	camera.setFarClip(DEFAULT_FAR);
	camera.disableOrtho();

	longitude = 0;
	latitude = 0;
	radius = (ofGetHeight() / 2.f) / tan(ofDegToRad(DEFAULT_FOV) / 2.f);
	baseRadius = radius;
	target = { ofGetWidth() / 2.f, ofGetHeight() / 2.f, 0 };
	lastFocusedObject = nullptr;
	focusCloseDistance = false;
	apply();
}

void SceneCamera::apply() {
	camera.orbitDeg(longitude, latitude, radius, target);
	camera.lookAt(target);
	float s = camera.getOrtho() ? radius / baseRadius : 1.f;
	camera.setScale(s);
}

void SceneCamera::orbit(float dLon, float dLat) {
	longitude += dLon;
	latitude = ofClamp(latitude - dLat, -89.f, 89.f);
	apply();
}

void SceneCamera::zoom(float factor) {
	radius = ofClamp(radius * factor, 20.f, 20000.f);
	apply();
}

void SceneCamera::pan(float dx, float dy) {
	target += camera.getSideDir() * dx - camera.getUpDir() * dy;
	apply();
}

void SceneCamera::focusOnObject(const SceneObject * obj) {
	if (!obj) return;

	const SceneModel3d * model = dynamic_cast<const SceneModel3d *>(obj);
	if (!model) return;

	if (obj != lastFocusedObject) {
		lastFocusedObject = obj;
		focusCloseDistance = false;
	} else {
		focusCloseDistance = !focusCloseDistance;
	}

	target = model->worldCenter();

	const float extent = model->worldBoundingRadius();
	const float fovRad = ofDegToRad(camera.getFov());
	float framedRadius = extent / tan(fovRad * 0.5f) * FOCUS_FRAME_MARGIN;
	framedRadius = ofClamp(framedRadius, MIN_ORBIT_RADIUS, MAX_ORBIT_RADIUS);

	radius = focusCloseDistance ? framedRadius * FOCUS_CLOSE_FACTOR : framedRadius;
	baseRadius = radius;
	apply();
}

void SceneCamera::setFov(float f) { camera.setFov(ofClamp(f, 10.f, 120.f)); }
void SceneCamera::setClip(float n, float f) {
	n = std::max(n, 0.01f);
	camera.setNearClip(n);
	camera.setFarClip(std::max(f, n + 1.f));
}
void SceneCamera::setOrtho(bool on) {
	if (on == camera.getOrtho()) return;
	on ? camera.enableOrtho() : camera.disableOrtho();
	apply();
}

glm::vec2 SceneCamera::screenToPlane(const glm::vec2 & screen, float planeZ) const {
	glm::vec3 a = camera.screenToWorld({ screen.x, screen.y, -1.f });
	glm::vec3 b = camera.screenToWorld({ screen.x, screen.y, 1.f });
	glm::vec3 dir = b - a;
	if (std::abs(dir.z) < 1e-6f) return { a.x, a.y };
	float t = (planeZ - a.z) / dir.z;
	glm::vec3 p = a + dir * t;
	return { p.x, p.y };
}
