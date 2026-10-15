#include "CameraController.h"

CameraController::CameraController(SceneCamera & camera)
	: cam(camera) { }

void CameraController::update(float dt) {
	float dx = (keyRight ? 1.f : 0.f) - (keyLeft ? 1.f : 0.f);
	float dy = (keyUp ? 1.f : 0.f) - (keyDown ? 1.f : 0.f);

	if (dx != 0.f || dy != 0.f) {
		float step = KEY_PAN_SPEED * cam.getRadius() * dt;
		cam.pan(dx * step, dy * step);
	}
}

void CameraController::mousePressed(int x, int y, int button, bool overGui) {
	if (overGui) {
		dragging = false;
		return;
	}
	dragging = true;
	lastX = x;
	lastY = y;
}

void CameraController::mouseDragged(int x, int y, int button) {
	if (!dragging) return;

	float dx = static_cast<float>(x - lastX);
	float dy = static_cast<float>(y - lastY);
	lastX = x;
	lastY = y;

	bool shift = ofGetKeyPressed(OF_KEY_SHIFT);
	if (button == OF_MOUSE_BUTTON_RIGHT && !shift) {
		cam.orbit(-dx * ORBIT_SENSITIVITY, dy * ORBIT_SENSITIVITY);
	} else if (button == OF_MOUSE_BUTTON_MIDDLE || (button == OF_MOUSE_BUTTON_RIGHT && shift)) {
		float scale = PAN_SENSITIVITY * cam.getRadius();
		cam.pan(-dx * scale, dy * scale);
	}
}

void CameraController::mouseReleased(int x, int y, int button) {
	dragging = false;
}

void CameraController::mouseScrolled(float scrollY) {
	if (scrollY > 0.f) {
		cam.zoom(ZOOM_STEP);
	} else if (scrollY < 0.f) {
		cam.zoom(1.f / ZOOM_STEP);
	}
}

void CameraController::keyPressed(int key) {
	int k = (key < 256) ? std::tolower(key) : key;

	switch (k) {
	case 'w':
		keyUp = true;
		break;
	case 's':
		keyDown = true;
		break;
	case 'a':
		keyLeft = true;
		break;
	case 'd':
		keyRight = true;
		break;
	default:
		break;
	}
}

void CameraController::keyReleased(int key) {
	int k = (key < 256) ? std::tolower(key) : key;

	switch (k) {
	case 'w':
		keyUp = false;
		break;
	case 's':
		keyDown = false;
		break;
	case 'a':
		keyLeft = false;
		break;
	case 'd':
		keyRight = false;
		break;
	default:
		break;
	}
}
