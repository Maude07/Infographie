#include "scenePrimitive3d.h"

namespace {

    constexpr float viewTilt = 20.0f;
}

glm::mat4 ScenePrimitive3d::localTransform() const {
    const float side = min(size.x, size.y);
    return glm::translate(getCenter3d())
        * glm::rotate(glm::radians(-viewTilt), glm::vec3(1, 0, 0))
        * glm::rotate(glm::radians(rotationY), glm::vec3(0, 1, 0))
        * glm::scale(glm::vec3(side, -side, side));
}

ofRectangle ScenePrimitive3d::getScreenBounds() const {
    const float side = min(size.x, size.y);
    if (!camera || side <= 0.0f) return getBounds();

    const glm::mat4 transform = localTransform();
    ofRectangle bounds;
    bool first = true;
    for (const glm::vec3 & vertex : mesh().getVertices()) {
        glm::vec3 world = transform * glm::vec4(vertex, 1.0f);
        glm::vec3 screen = camera->worldToScreen(world);
        if (first) { bounds.set(screen.x, screen.y, 0, 0); first = false; }
        else bounds.growToInclude(screen.x, screen.y);
    }
    return bounds;
}


void ScenePrimitive3d::draw() const {
    const glm::vec2 center = position + size * 0.5f;
    const float side = min(size.x, size.y);
    if (side <= 0.0f) return;

    static ofLight light;
    light.setPointLight();
    light.setDiffuseColor(ofColor(255));
    light.setPosition(center.x - side, center.y - side, depth + side * 3.0f);

    static ofMaterial material;
    material.setDiffuseColor(color);
    material.setAmbientColor(color * 0.3f);

    ofPushStyle();
    ofEnableLighting();
    light.enable();
    material.begin();

    ofPushMatrix();
    ofMultMatrix(localTransform());

    ofEnableDepthTest();
    mesh().draw();
    ofDisableDepthTest();
    ofPopMatrix();

    material.end();
    light.disable();
    ofDisableLighting();
    ofPopStyle();
}

void ScenePrimitive3d::addTriangle(ofMesh & mesh,
    const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
    const glm::vec3 & na, const glm::vec3 & nb, const glm::vec3 & nc) {
        mesh.addVertex(a);
        mesh.addNormal(na);
        mesh.addVertex(b);
        mesh.addNormal(nb);
        mesh.addVertex(c);
        mesh.addNormal(nc);
}

glm::vec3 ScenePrimitive3d::circlePoint(int segment) {
    float phi = TWO_PI * segment / segments;
    return glm::vec3(cos(phi), 0.0f, sin(phi));
}

void ScenePrimitive3d::addCap(ofMesh & mesh, float y, const glm::vec3 & normal) {
    glm::vec3 center(0.0f, y, 0.0f);
    for (int segment = 0; segment < segments; ++ segment) {
        glm::vec3 point0 = circlePoint(segment) * half + center;
        glm::vec3 point1 = circlePoint(segment + 1) * half + center;
        addTriangle(mesh, center, point0, point1, normal, normal, normal);
    }
}
