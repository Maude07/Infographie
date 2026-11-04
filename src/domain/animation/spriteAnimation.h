#pragma once
#include "ofMain.h"

struct SpriteAnimation {
	std::string name;
	std::string sheetPath;
	int frameWidth = 0;
	int frameHeight = 0;
	int columns = 1;
	int firstFrame = 0;
	int frameCount = 1;
	float fps = 12.0f;

	ofRectangle frameRect(int n) const {
		int index = firstFrame + n;
		return ofRectangle(
			static_cast<float>((index % columns) * frameWidth),
			static_cast<float>((index / columns) * frameHeight),
			static_cast<float>(frameWidth),
			static_cast<float>(frameHeight));
	}

	float aspectRatio() const {
		return frameHeight > 0 ? static_cast<float>(frameWidth) / frameHeight : 1.0f;
	}
};
