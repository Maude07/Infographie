#include "domain/shapes/3d/scenePrimitiveCylinder.h"

const ofMesh & ScenePrimitiveCylinder::mesh() const {
    static const ofMesh cylinder = generate();
    return cylinder;
}

ofMesh ScenePrimitiveCylinder::generate() {
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    const glm::vec3 halfHeight(0, half, 0);
    for (int segment = 0; segment < segments; ++segment) {
        glm::vec3 radial = circlePoint(segment);
        glm::vec3 nextRadial = circlePoint(segment + 1);

        glm::vec3 bottom = radial * half - halfHeight;
        glm::vec3 nextBottom = nextRadial * half - halfHeight;
        glm::vec3 top = radial * half + halfHeight;
        glm::vec3 nextTop = nextRadial * half + halfHeight;

        addTriangle(mesh, bottom, nextBottom, nextTop, radial, nextRadial, nextRadial);
        addTriangle(mesh, bottom, nextTop, top, radial, nextRadial, radial);
    }

    addCap(mesh, half, glm::vec3(0, 1, 0));
    addCap(mesh, -half, glm::vec3(0, -1, 0));
    return mesh;
}