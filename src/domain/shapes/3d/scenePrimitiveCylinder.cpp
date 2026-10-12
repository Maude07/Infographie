#include "domain/shapes/3d/scenePrimitiveCylinder.h"

const ofMesh & ScenePrimitiveCylinder::mesh() const {
    static const ofMesh cylinder = generate();
    return cylinder;
}

//TODO : Change variable names
ofMesh ScenePrimitiveCylinder::generate() {
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    const glm::vec3 up(0, half, 0);
    for (int segment = 0; segment < segments; ++segment) {
        glm::vec3 d0 = circlePoint(segment);
        glm::vec3 d1 = circlePoint(segment + 1);

        glm::vec3 bottom0 = d0 * half - up;
        glm::vec3 bottom1 = d1 * half - up;
        glm::vec3 top0 = d0 * half + up;
        glm::vec3 top1 = d1 * half + up;

        addTriangle(mesh, bottom0, bottom1, top1, d0, d1, d1);
        addTriangle(mesh, bottom0, top1, top0, d0, d1, d0);
    }

    addCap(mesh, half, glm::vec3(0, 1, 0));
    addCap(mesh, -half, glm::vec3(0, -1, 0));
    return mesh;
}