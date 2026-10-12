#include "domain/shapes/3d/scenePrimitiveCube.h"

const ofMesh & ScenePrimitiveCube::mesh() const {
    static const ofMesh cube = generate();
    return cube;
}
    
ofMesh ScenePrimitiveCube::generate() {
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    const glm::vec3 faceNormals[] = {
        { 1, 0, 0 }, { -1, 0, 0 },
        { 0, 1, 0 }, { 0, -1, 0 },
        { 0, 0, 1 }, { 0, 0, -1 }
    };

    //TOOD : CHange variable names
    for (const glm::vec3 & normal : faceNormals) {
        glm::vec3 u = abs(normal.y) > 0.5f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        glm::vec3 v = glm::cross(normal, u);
        glm::vec3 c = normal * half;

        glm::vec3 point0 = c - u * half - v * half;
        glm::vec3 point1 = c + u * half - v * half;
        glm::vec3 point2 = c + u * half + v * half;
        glm::vec3 point3 = c - u * half + v * half;

        addTriangle(mesh, point0, point1, point2, normal, normal, normal);
        addTriangle(mesh, point0, point2, point3, normal, normal, normal);
    }
    return mesh;
}