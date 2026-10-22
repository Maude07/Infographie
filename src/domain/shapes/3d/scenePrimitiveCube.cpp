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

    for (const glm::vec3 & normal : faceNormals) {
        glm::vec3 tangent = abs(normal.y) > 0.5f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        glm::vec3 bitangent = glm::cross(normal, tangent);
        glm::vec3 faceCenter = normal * half;

        glm::vec3 bottomLeft = faceCenter - tangent * half - bitangent * half;
        glm::vec3 bottomRight = faceCenter + tangent * half - bitangent * half;
        glm::vec3 topRight = faceCenter + tangent * half + bitangent * half;
        glm::vec3 topLeft = faceCenter - tangent * half + bitangent * half;

        addTriangle(mesh, bottomLeft, bottomRight, topRight, normal, normal, normal);
        addTriangle(mesh, bottomLeft, topRight, topLeft, normal, normal, normal);
    }
    return mesh;
}