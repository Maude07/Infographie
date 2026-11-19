#pragma once

#include "ofMain.h"
#include "domain/shapes/3d/sceneParametricSurface.h"
#include <array>

class ScenePrimitiveBezierSurface : public SceneParametricSurface {
public:
    static constexpr int controlCount = 4;

    std::array<glm::vec3, controlCount * controlCount> controlPoints() const;
    glm::vec3 evaluate(float u, float v) const override;
    glm::vec3 normalAt(float u, float v) const override;

protected:
    void drawControlNet() const override;
};
