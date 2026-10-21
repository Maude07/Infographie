#pragma once

#include "ofMain.h"
#include "../../scene/sceneObject.h"

class ScenePrimitive3d : public SceneObject {
public: 
    ofColor color = ofColor::white;
    float rotationY = 0.0f;
    float depth = 0.0f;

    void draw() const override;

    ofRectangle getScreenBounds() const override;
    
    void setBounds(const ofRectangle & bounds) override {
        float side = max(bounds.width, bounds.height);
        SceneObject::setBounds(ofRectangle(bounds.x, bounds.y, side, side));
    }

protected:
    static constexpr float half = 0.5f;
    static constexpr int segments = 32;

    virtual const ofMesh & mesh() const = 0;

    static void addTriangle(ofMesh & mesh,
        const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
        const glm::vec3 & na, const glm::vec3 & nb, const glm::vec3 & nc);
    static glm::vec3 circlePoint(int segment);
    static void addCap(ofMesh & mesh, float y, const glm::vec3 & normal);
};
