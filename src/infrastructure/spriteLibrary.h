#pragma once
#include "ofMain.h"
#include "domain/animation/spriteAnimation.h"

using namespace std;

class SpriteLibrary {
public:
	static constexpr const char * generatedFolder = "sprites/generated";

	void setup();

	size_t add(const SpriteAnimation & animation, shared_ptr<ofImage> sheet);

	const vector<SpriteAnimation> & animations() const { return catalog; }
	shared_ptr<ofImage> sheetFor(const SpriteAnimation & animation) const;

private:
	bool load(const SpriteAnimation & animation);
	void loadGenerated();

	vector<SpriteAnimation> catalog;
	map<string, shared_ptr<ofImage>> sheets;
};
