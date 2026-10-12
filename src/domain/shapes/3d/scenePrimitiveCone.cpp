#include "domain/shapes/3d/scenePrimitiveCone.h"

const ofMesh & ScenePrimitiveCone::mesh() const {
    static const ofMesh cone = generate();
    return cone;
}

//TODO : Change variable names
    ofMesh ScenePrimitiveCone::generate() {
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);

        const glm::vec3 apex(0, half, 0);
        const float height = 2.0f * half;
        const float radius = half;

        auto sideNormal = [&](const glm::vec3 & d) {
            return glm::normalize(d * height + glm::vec3(0, radius, 0));
        };

        for (int segment = 0; segment < segments; ++segment) {
            glm::vec3 d0 = circlePoint(segment);
            glm::vec3 d1 = circlePoint(segment + 1);

            glm::vec3 base0 = d0 * radius - glm::vec3(0, half, 0);
            glm::vec3 base1 = d1 * radius - glm::vec3(0, half, 0);

            glm::vec3 n0 = sideNormal(d0);
            glm::vec3 n1 = sideNormal(d1);
            glm::vec3 nApex = glm::normalize(n0 + n1);

            addTriangle(mesh, base0, base1, apex, n0, n1, nApex);
        }

        addCap(mesh, -half, glm::vec3(0, -1, 0));
        return mesh;
    }