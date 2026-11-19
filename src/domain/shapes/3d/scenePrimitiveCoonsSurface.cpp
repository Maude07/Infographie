#include "domain/shapes/3d/scenePrimitiveCoonsSurface.h"

namespace {
    constexpr int guideSamples = 48;
}

glm::vec3 ScenePrimitiveCoonsSurface::bottom(float u) const {
    return { -half + u, amplitude * 0.8f * sin(PI * u), -half };
}

glm::vec3 ScenePrimitiveCoonsSurface::top(float u) const {
    return { -half + u, -amplitude * 0.6f * sin(TWO_PI * u), half };
}

glm::vec3 ScenePrimitiveCoonsSurface::left(float v) const {
    return { -half, amplitude * 0.5f * sin(TWO_PI * v), -half + v };
}

glm::vec3 ScenePrimitiveCoonsSurface::right(float v) const {
    return { half, amplitude * 0.9f * sin(PI * v), -half + v };
}

glm::vec3 ScenePrimitiveCoonsSurface::evaluate(float u, float v) const {
    const glm::vec3 ruledV = (1.0f - v) * bottom(u) + v * top(u);
    const glm::vec3 ruledU = (1.0f - u) * left(v) + u * right(v);
    const glm::vec3 bilinear =
        (1.0f - u) * (1.0f - v) * bottom(0.0f)
        + u * (1.0f - v) * bottom(1.0f)
        + (1.0f - u) * v * top(0.0f)
        + u * v * top(1.0f);
    return ruledV + ruledU - bilinear;
}

void ScenePrimitiveCoonsSurface::drawControlNet() const {
    auto drawBoundary = [](auto curve) {
        ofPolyline line;
        for (int k = 0; k <= guideSamples; ++k) line.addVertex(curve(static_cast<float>(k) / guideSamples));
        line.draw();
    };
    drawBoundary([this](float t) { return bottom(t); });
    drawBoundary([this](float t) { return top(t); });
    drawBoundary([this](float t) { return left(t); });
    drawBoundary([this](float t) { return right(t); });

    for (const glm::vec3 & corner : { bottom(0.0f), bottom(1.0f), top(0.0f), top(1.0f) }) {
        ofDrawSphere(corner, guideRadius);
    }
}
