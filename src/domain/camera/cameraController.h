#pragma once

#include "SceneCamera.h"
#include "ofMain.h"

class CameraController {
public:
	explicit CameraController(SceneCamera & camera);

	void update(float dt);

	void mousePressed(int x, int y, int button, bool overGui);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(float scrollY);

	void keyPressed(int key);
	void keyReleased(int key);

private:
	SceneCamera & cam;

	bool dragging = false;
	int lastX = 0;
	int lastY = 0;

	bool keyUp = false;
	bool keyDown = false;
	bool keyLeft = false;
	bool keyRight = false;

	static constexpr float ORBIT_SENSITIVITY = 0.3f;
	static constexpr float PAN_SENSITIVITY = 0.002f;
	static constexpr float ZOOM_STEP = 0.9f;
	static constexpr float KEY_PAN_SPEED = 0.5f;
};
