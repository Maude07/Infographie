#pragma once
#include "ofMain.h"
#include "../../scene/sceneObject.h"

enum class Primitive3dType {
    Cube,
    Sphere,
    Cylinder,
    Cone
};

string primitive3dLabel(Primitive3dType type);

class ScenePrimitive3d : public SceneObject {
public: 
    ScenePrimitive3d(Primitive3dType type,
        const glm::vec2 & center,
        float footprint = 150.0f);    

    void draw() const override;

    void setBounds(const ofRectangle & bounds) override {
        float side = max(bounds.width, bounds.height);
        SceneObject::setBounds(ofRectangle(bounds.x, bounds.y, side, side));
    }

    Primitive3dType getType() const { return type; }

    ofColor color = ofColor(255);
    float rotationY = 0.0f;
    float depth = 0.0f;

private:
    Primitive3dType type;
};
