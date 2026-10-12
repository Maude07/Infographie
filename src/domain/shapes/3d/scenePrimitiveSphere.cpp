#include "domain/shapes/3d/scenePrimitiveSphere.h"

namespace {
    constexpr int rings = 16;
}

const ofMesh & ScenePrimitiveSphere::mesh() const {
    static const ofMesh sphere = generate();
    return sphere;
}

//TODO : Change variable names
ofMesh ScenePrimitiveSphere::generate() {
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    auto direction = [](int ring, int segment) {
        float theta = PI * ring / rings;
        float phi = TWO_PI * segment / segments;
        return glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
    };

    for (int ring = 0; ring < rings; ++ring) {
        for (int segment = 0; segment < segments; ++segment) {
            glm::vec3 n00 = direction(ring, segment);
            glm::vec3 n01 = direction(ring, segment + 1);
            glm::vec3 n10 = direction(ring + 1, segment);
            glm::vec3 n11 = direction(ring + 1, segment + 1);

            addTriangle(mesh, n00 * half, n10 * half, n11 * half, n00, n10, n11);
            addTriangle(mesh, n00 * half, n11 * half, n01 * half, n00, n11, n01);
        }
    }
    return mesh;
}