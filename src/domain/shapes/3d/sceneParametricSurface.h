#pragma once

#include "ofMain.h"
#include "domain/shapes/3d/scenePrimitive3d.h"

class SceneParametricSurface : public ScenePrimitive3d {
public:
    static constexpr int minResolution = 4;
    static constexpr int maxResolution = 64;

    float amplitude = 0.35f;
    int resolution = 24;
    bool showControlNet = true;

    void draw() const override;

    virtual glm::vec3 evaluate(float u, float v) const = 0;
    virtual glm::vec3 normalAt(float u, float v) const;

protected:
    static constexpr float guideRadius = 0.015f;

    const ofMesh & mesh() const override;

    virtual void drawControlNet() const = 0;

private:
    static constexpr float shellOffset = 0.004f;

    void rebuild() const;

    mutable ofMesh cachedMesh;
    mutable float cachedAmplitude = -1.0f;
    mutable int cachedResolution = 0;
};
