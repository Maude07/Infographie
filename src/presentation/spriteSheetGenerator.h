#pragma once
#include "ofMain.h"
#include "domain/scene/sceneObject.h"
#include "domain/animation/spriteAnimation.h"

using namespace std;

class SpriteSheetGenerator {
public:
	struct Result {
		SpriteAnimation animation;
		shared_ptr<ofImage> sheet;
	};

	bool generateRotation(SceneObject & object, int frameCount, int cellSize,
		const string & outputFolder, Result & result);

private:
	void renderFrame2d(const SceneObject & object, float angle, int cellSize);
	void renderFrame3d(SceneObject & object, float angle, int cellSize);

	ofFbo cellFbo;
};
