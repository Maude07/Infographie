#include "domain/scene/sceneSprite.h"

SceneSprite::SceneSprite(shared_ptr<ofImage> s, const SpriteAnimation & a)
	: fps(a.fps)
	, sheet(move(s))
	, animation(a) {
	name = animation.name;
	size = { static_cast<float>(animation.frameWidth), static_cast<float>(animation.frameHeight) };
}

void SceneSprite::advance(float deltaSeconds) {
	if (!playing || animation.frameCount <= 0) return;
	phase = fmod(phase + deltaSeconds * fps, static_cast<float>(animation.frameCount));
}

int SceneSprite::currentFrame() const {
	return static_cast<int>(phase) % max(1, animation.frameCount);
}

void SceneSprite::draw() const {
	if (!sheet || !sheet->isAllocated()) return;

	ofRectangle source = animation.frameRect(currentFrame());

	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255);
	sheet->drawSubsection(position.x, position.y, size.x, size.y,
		source.x, source.y, source.width, source.height);
	ofPopStyle();
}
