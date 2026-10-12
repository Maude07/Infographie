#include "scenePrimitive3d.h"

namespace {

    constexpr float half = 0.5f;
    constexpr int segments = 32;
    constexpr int rings = 16;

    constexpr float viewTilt = 20.0f;

    void addTriangle(ofMesh & mesh,
        const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
        const glm::vec3 & na, const glm::vec3 & nb, const glm::vec3 & nc) {
            mesh.addVertex(a);
            mesh.addNormal(na);
            mesh.addVertex(b);
            mesh.addNormal(nb);
            mesh.addVertex(c);
            mesh.addNormal(nc);
        }

    glm::vec3 circlePoint(int segment) {
        float phi = TWO_PI * segment / segments;
        return glm::vec3(cos(phi), 0.0f, sin(phi));
    }

    void addCap(ofMesh & mesh, float y, const glm::vec3 & normal) {
        glm::vec3 center(0.0f, y, 0.0f);
        for (int segment = 0; segment < segments; ++segment) {
            glm::vec3 point0 = circlePoint(segment) * half + center;
            glm::vec3 point1 = circlePoint(segment + 1) * half + center;
            addTriangle(mesh, center, point0, point1, normal, normal, normal);
        }
    }

    ofMesh makeCube() {
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

    //TODO : Change variable names
    ofMesh makeSphere() {
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

    //TODO : Change variable names
    ofMesh makeCylinder() {
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

    //TODO : Change variable names
    ofMesh makeCone() {
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

    const ofMesh & meshFor(Primitive3dType type) {
        static const ofMesh cube = makeCube();
        static const ofMesh sphere = makeSphere();
        static const ofMesh cylinder = makeCylinder();
        static const ofMesh cone = makeCone();

        switch (type) {
            case Primitive3dType::Cube:
                return cube;
            case Primitive3dType::Sphere:
                return sphere;
            case Primitive3dType::Cylinder:
                return cylinder;
            case Primitive3dType::Cone:
                return cone;
        }
        return cube;
    }
}

string primitive3dLabel(Primitive3dType type) {
    switch (type) {
        case Primitive3dType::Cube:
            return "cube";
        case Primitive3dType::Sphere:
            return "sphere";
        case Primitive3dType::Cylinder:
            return "cylindre";
        case Primitive3dType::Cone:
            return "cone";
    }
    return "primitive 3d";
}

ScenePrimitive3d::ScenePrimitive3d(Primitive3dType type,
    const glm::vec2 & center,
    float footprint): type(type) {
        name = primitive3dLabel(type);
        size = { footprint, footprint };
        position = center - size * 0.5f;
}

void ScenePrimitive3d::draw() const {
    const glm::vec2 center = position + size * 0.5f;
    const float side = min(size.x, size.y);
    if (side <= 0.0f) return;

    static ofLight light;
    light.setPointLight();
    light.setDiffuseColor(ofColor(255));
    light.setPosition(center.x - side, center.y - side, side * 3.0f);

    static ofMaterial material;
    material.setDiffuseColor(color);
    material.setAmbientColor(color * 0.3f);

    ofPushStyle();
    ofEnableLighting();
    light.enable();
    material.begin();

    ofPushMatrix();
    ofTranslate(center.x, center.y, depth);
    ofRotateXDeg(-viewTilt);
    ofRotateYDeg(rotationY);
    ofScale(side, -side, side);

    glClear(GL_DEPTH_BUFFER_BIT);
    ofEnableDepthTest();
    meshFor(type).draw();
    ofDisableDepthTest();
    ofPopMatrix();

    material.end();
    light.disable();
    ofDisableLighting();
    ofPopStyle();
}



