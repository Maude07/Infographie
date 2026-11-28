#pragma once

#include "ofMain.h"
#include "domain/camera/sceneCamera.h"

struct SdfOptions {
    float size = 120.0f;
    float blend = 0.0f;
    bool animate = true;
};

class SdfRenderer {
public:
    void setup();
    void draw(const SceneCamera & camera, const ofRectangle & canvas, const SdfOptions & options);

private:
    ofShader shader;
};
