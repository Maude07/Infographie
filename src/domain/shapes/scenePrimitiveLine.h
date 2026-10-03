#pragma once
#include "domain/shapes/scenePrimitive.h"
#include "ofMain.h"

class ScenePrimitiveLine : public ScenePrimitive {
public:
	void draw() const override;
	ofRectangle getBounds() const override;
	bool contains(float x, float y) const override;
};
