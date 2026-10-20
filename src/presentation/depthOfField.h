#pragma once

#include "ofMain.h"
#include "domain/camera/sceneCamera.h"

class DepthOfField {
public:
    void setup();

    void begin();
    void end();
    
    void draw(const SceneCamera & camera, float focusDistance, float focusRange, float maxBlur);

private:
    ofFbo fbo;
    ofShader shader;

    void allocate(int width, int height);
};
