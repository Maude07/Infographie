#pragma once

#include "ofMain.h"

using namespace std;

class ImageExporter {
public:
	bool isRecording = false;
	int fps = 24;
	float duration = 5.0f;
	int frameCount = 0;

	int captureX = 0;
	int captureY = 0;
	int captureWidth = 0;
	int captureHeight = 0;

	void start();
	void stop();
	void toggle();
	void capture();

private:
	string folder;
	float startTime = 0.0f;
	float lastCaptureTime = -1.0f;
	ofImage image;
};
