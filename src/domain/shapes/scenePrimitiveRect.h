#pragma once
#include "domain/shapes/scenePrimitive.h"
#include "ofMain.h"

class ScenePrimitiveRect : public ScenePrimitive {
public:
	void draw() const override;
};
