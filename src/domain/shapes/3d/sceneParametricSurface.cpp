#include "domain/shapes/3d/sceneParametricSurface.h"

glm::vec3 SceneParametricSurface::normalAt(float u, float v) const {
    constexpr float h = 1e-3f;
    const glm::vec3 tangentU = evaluate(std::min(u + h, 1.0f), v) - evaluate(std::max(u - h, 0.0f), v);
    const glm::vec3 tangentV = evaluate(u, std::min(v + h, 1.0f)) - evaluate(u, std::max(v - h, 0.0f));
    const glm::vec3 normal = glm::cross(tangentV, tangentU);
    const float length = glm::length(normal);
    return length > 1e-9f ? normal / length : glm::vec3(0.0f, 1.0f, 0.0f);
}

const ofMesh & SceneParametricSurface::mesh() const {
    if (cachedAmplitude != amplitude || cachedResolution != resolution) rebuild();
    return cachedMesh;
}

void SceneParametricSurface::rebuild() const {
    const int steps = std::clamp(resolution, minResolution, maxResolution);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    positions.reserve((steps + 1) * (steps + 1));
    normals.reserve((steps + 1) * (steps + 1));
    for (int i = 0; i <= steps; ++i) {
        for (int j = 0; j <= steps; ++j) {
            const float u = static_cast<float>(i) / steps;
            const float v = static_cast<float>(j) / steps;
            positions.push_back(evaluate(u, v));
            normals.push_back(normalAt(u, v));
        }
    }

    cachedMesh.clear();
    cachedMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    auto index = [steps](int i, int j) { return i * (steps + 1) + j; };
    auto uv = [steps](int i, int j) { return glm::vec2(static_cast<float>(i) / steps, static_cast<float>(j) / steps); };

    for (float side : { 1.0f, -1.0f }) {
        for (int i = 0; i < steps; ++i) {
            for (int j = 0; j < steps; ++j) {
                const int quad[4] = { index(i, j), index(i + 1, j), index(i + 1, j + 1), index(i, j + 1) };
                const glm::vec2 quadUv[4] = { uv(i, j), uv(i + 1, j), uv(i + 1, j + 1), uv(i, j + 1) };

                auto point = [&](int k) { return positions[quad[k]] + normals[quad[k]] * (shellOffset * side); };
                auto normal = [&](int k) { return normals[quad[k]] * side; };

                addTriangle(cachedMesh, point(0), point(1), point(2), normal(0), normal(1), normal(2));
                addTriangle(cachedMesh, point(0), point(2), point(3), normal(0), normal(2), normal(3));
                addTexCoords(cachedMesh, quadUv[0], quadUv[1], quadUv[2]);
                addTexCoords(cachedMesh, quadUv[0], quadUv[2], quadUv[3]);
            }
        }
    }

    cachedAmplitude = amplitude;
    cachedResolution = resolution;
}

void SceneParametricSurface::draw() const {
    ScenePrimitive3d::draw();
    if (!showControlNet || min(size.x, size.y) <= 0.0f) return;

    ofPushStyle();
    ofPushMatrix();
    ofMultMatrix(localTransform());
    ofEnableDepthTest();
    ofSetColor(255, 210, 60);
    ofSetLineWidth(1.5f);

    drawControlNet();

    ofDisableDepthTest();
    ofPopMatrix();
    ofPopStyle();
}
