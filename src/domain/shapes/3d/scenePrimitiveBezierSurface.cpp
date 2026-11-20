#include "domain/shapes/3d/scenePrimitiveBezierSurface.h"

namespace {

    constexpr float heights[ScenePrimitiveBezierSurface::controlCount][ScenePrimitiveBezierSurface::controlCount] = {
        { 0.0f, 0.3f, -0.2f, 0.0f },
        { 0.4f, 1.0f, -0.6f, 0.2f },
        { -0.3f, -0.8f, 0.9f, -0.1f },
        { 0.0f, 0.2f, -0.3f, 0.0f }
    };

    std::array<float, 4> bernstein(float t) {
        const float s = 1.0f - t;
        return { s * s * s, 3.0f * t * s * s, 3.0f * t * t * s, t * t * t };
    }

    std::array<float, 4> bernsteinDerivative(float t) {
        const float s = 1.0f - t;
        return { -3.0f * s * s, 3.0f * s * s - 6.0f * t * s, 6.0f * t * s - 3.0f * t * t, 3.0f * t * t };
    }
}

std::array<glm::vec3, ScenePrimitiveBezierSurface::controlCount * ScenePrimitiveBezierSurface::controlCount>
ScenePrimitiveBezierSurface::controlPoints() const {
    std::array<glm::vec3, controlCount * controlCount> points;
    for (int i = 0; i < controlCount; ++i) {
        for (int j = 0; j < controlCount; ++j) {
            const float x = -half + static_cast<float>(i) / (controlCount - 1);
            const float z = -half + static_cast<float>(j) / (controlCount - 1);
            points[i * controlCount + j] = { x, heights[i][j] * amplitude, z };
        }
    }
    return points;
}

glm::vec3 ScenePrimitiveBezierSurface::evaluate(float u, float v) const {
    const auto points = controlPoints();
    const auto bu = bernstein(u);
    const auto bv = bernstein(v);
    glm::vec3 point(0.0f);
    for (int i = 0; i < controlCount; ++i) {
        for (int j = 0; j < controlCount; ++j) {
            point += bu[i] * bv[j] * points[i * controlCount + j];
        }
    }
    return point;
}

glm::vec3 ScenePrimitiveBezierSurface::normalAt(float u, float v) const {
    const auto points = controlPoints();
    const auto bu = bernstein(u);
    const auto bv = bernstein(v);
    const auto du = bernsteinDerivative(u);
    const auto dv = bernsteinDerivative(v);
    glm::vec3 tangentU(0.0f);
    glm::vec3 tangentV(0.0f);
    for (int i = 0; i < controlCount; ++i) {
        for (int j = 0; j < controlCount; ++j) {
            const glm::vec3 & p = points[i * controlCount + j];
            tangentU += du[i] * bv[j] * p;
            tangentV += bu[i] * dv[j] * p;
        }
    }
    const glm::vec3 normal = glm::cross(tangentV, tangentU);
    const float length = glm::length(normal);
    return length > 1e-6f ? normal / length : glm::vec3(0.0f, 1.0f, 0.0f);
}

void ScenePrimitiveBezierSurface::drawControlNet() const {
    const auto points = controlPoints();
    for (int i = 0; i < controlCount; ++i) {
        for (int j = 0; j < controlCount; ++j) {
            const glm::vec3 & p = points[i * controlCount + j];
            if (i + 1 < controlCount) ofDrawLine(p, points[(i + 1) * controlCount + j]);
            if (j + 1 < controlCount) ofDrawLine(p, points[i * controlCount + j + 1]);
        }
    }
    for (const glm::vec3 & p : points) ofDrawSphere(p, guideRadius);
}
