#pragma once
#include "ofMain.h"

using namespace std;

inline glm::vec2 rotateVector(const glm::vec2 & v, float degrees) {
    float r = glm::radians(degrees);
    float c = cos(r), s = sin(r);
    return { v.x * c - v.y * s, v.x * s + v.y * c };
}

class SceneObject {
public:
    string name;
    glm::vec2 position;
    glm::vec2 size;
    float rotation = 0.0f;

    static constexpr float minSize = 20.0f;

    virtual ~SceneObject() = default;
    virtual void draw() const = 0;

    virtual ofRectangle getBounds() const { 
        return ofRectangle(position.x, position.y, size.x, size.y); 
    }

    virtual void setBounds(const ofRectangle & bounds) {
        position = {bounds.x, bounds.y };
        size = { bounds.width, bounds.height };
    }

    virtual bool contains(float x, float y) const { 
        return getBounds().inside(x, y); 
    }

    virtual bool isResizable() const { return true; }
    virtual bool isRotatable() const { return true; }
    
    glm::vec2 getCenter() const {
        auto center = getBounds().getCenter();
        return { center.x, center.y };
    }

    void applyTransform() const {
        glm::vec2 center = getCenter();
        ofTranslate(center.x, center.y);
        ofRotateDeg(rotation);
        ofTranslate(-center.x, -center.y);
    }

    void render() const {
        ofPushMatrix();
        applyTransform();
        draw();
        ofPopMatrix();
    }

    glm::vec2 toLocal(float x, float y) const {
        glm::vec2 center = getCenter();
        return center + rotateVector(glm::vec2(x, y) - center, -rotation);
    }

    glm::vec2 toWorld(const glm::vec2 & local) const {
        glm::vec2 center = getCenter();
        return center + rotateVector(local - center, rotation);
    }

    bool hitTest(float x, float y) const {
        glm::vec2 localPoint = toLocal(x, y);
        return contains(localPoint.x, localPoint.y);
    }
};
