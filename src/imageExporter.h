#pragma once

#include "ofMain.h"

class ImageExporter {
public:
	bool is_recording = false;
	int fps = 24; // cadence d'exportation (images/seconde)
	float duration = 5.0f; // durée en secondes (0 = illimité)
	int frame_count = 0; // nombre d'images exportées

	void start();
	void stop();
	void toggle();
	void capture(); // à appeler après le rendu de la scène

private:
	std::string folder;
	float start_time = 0.0f;
	float last_capture_time = -1.0f;
	ofImage image;
};
