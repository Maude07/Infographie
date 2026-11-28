#pragma once

#include "ofMain.h"
#include "domain/scene/scene.h"
#include "domain/camera/sceneCamera.h"

struct RayTracingOptions {
    bool shadows = true;
    bool floor = true;
};

class RayTracer {
public:
    static constexpr int maxObjects = 16;
    static constexpr int maxLights = 8;

    void setup();
    void draw(const Scene & scene, const SceneCamera & camera, const ofRectangle & canvas,
        const RayTracingOptions & options);

private:
    ofShader shader;

    float uploadObjects(const Scene & scene);
    void uploadLights(const LightRig & rig);
};
