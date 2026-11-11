#pragma once
#include "domain/scene/sceneObject.h"
#include "domain/animation/spriteAnimation.h"

using namespace std;

class SceneSprite : public SceneObject {
public:
	SceneSprite(shared_ptr<ofImage> sheet, const SpriteAnimation & animation);

	void draw() const override;

	void advance(float deltaSeconds);
	int currentFrame() const;
	const SpriteAnimation & getAnimation() const { return animation; }

	float fps;
	bool playing = true;

private:
	shared_ptr<ofImage> sheet;
	SpriteAnimation animation;
	float phase = 0.0f;
};
