#pragma once
#include "domain/shapes/2d/scenePrimitive.h"
#include "ofMain.h"

using namespace std;

class ScenePrimitivePoint : public ScenePrimitive {
public:
	void draw() const override;
	ofRectangle getBounds() const override;
	void setBounds(const ofRectangle & bounds) override;
	bool isResizable() const override { return false; }
	bool contains(float x, float y) const override;

private:
	static constexpr float minRadius = 2.0f;
	float radius() const { return max(lineWidth, minRadius); }
};
