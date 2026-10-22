#include "domain/shapes/3d/scenePrimitiveCone.h"

const ofMesh & ScenePrimitiveCone::mesh() const {
    static const ofMesh cone = generate();
    return cone;
}

ofMesh ScenePrimitiveCone::generate() {
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    const glm::vec3 apex(0, half, 0);
    const float height = 2.0f * half;
    const float radius = half;

    auto sideNormal = [&](const glm::vec3 & radial) {
        return glm::normalize(radial * height + glm::vec3(0, radius, 0));
    };

    for (int segment = 0; segment < segments; ++segment) {
        glm::vec3 radial = circlePoint(segment);
        glm::vec3 nextRadial = circlePoint(segment + 1);

        glm::vec3 rim = radial * radius - glm::vec3(0, half, 0);
        glm::vec3 nextRim = nextRadial * radius - glm::vec3(0, half, 0);

        glm::vec3 rimNormal = sideNormal(radial);
        glm::vec3 nextRimNormal = sideNormal(nextRadial);
        glm::vec3 apexNormal = glm::normalize(rimNormal + nextRimNormal);

        addTriangle(mesh, rim, nextRim, apex, rimNormal, nextRimNormal, apexNormal);
    }

    addCap(mesh, -half, glm::vec3(0, -1, 0));
    return mesh;
}