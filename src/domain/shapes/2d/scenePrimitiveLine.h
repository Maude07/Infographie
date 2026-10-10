#pragma once
#include "domain/shapes/2d/scenePrimitive.h"
#include "ofMain.h"

class ScenePrimitiveLine : public ScenePrimitive {
public:
	void draw() const override;
	ofRectangle getBounds() const override;
	void setBounds(const ofRectangle & bounds) override;
	bool contains(float x, float y) const override;
};
