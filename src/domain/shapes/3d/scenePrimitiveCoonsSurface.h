#pragma once

#include "ofMain.h"
#include "domain/shapes/3d/sceneParametricSurface.h"

class ScenePrimitiveCoonsSurface : public SceneParametricSurface {
public:
    glm::vec3 bottom(float u) const;
    glm::vec3 top(float u) const;
    glm::vec3 left(float v) const;
    glm::vec3 right(float v) const;

    glm::vec3 evaluate(float u, float v) const override;

protected:
    void drawControlNet() const override;
};
