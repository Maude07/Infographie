#include "domain/shapes/3d/scenePrimitiveSphere.h"

namespace {
    constexpr int rings = 16;
}

const ofMesh & ScenePrimitiveSphere::mesh() const {
    static const ofMesh sphere = generate();
    return sphere;
}

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
            glm::vec3 topLeft = direction(ring, segment);
            glm::vec3 topRight = direction(ring, segment + 1);
            glm::vec3 bottomLeft = direction(ring + 1, segment);
            glm::vec3 bottomRight = direction(ring + 1, segment + 1);

            addTriangle(mesh, topLeft * half, bottomLeft * half,
                 bottomRight * half, topLeft, bottomLeft, bottomRight);
            addTriangle(mesh, topLeft * half, bottomRight * half,
                 topRight * half, topLeft, bottomRight, topRight);
        }
    }
    return mesh;
}