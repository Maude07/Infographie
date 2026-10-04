#pragma once
#include "ofMain.h"

using namespace std;

class SceneObject {
public:
    string name;
    glm::vec2 position;
    glm::vec2 size;

    virtual ~SceneObject() = default;
    virtual void draw() const = 0;

    virtual ofRectangle getBounds() const { 
        return ofRectangle(position.x, position.y, size.x, size.y); 
    }

    virtual bool contains(float x, float y) const { 
        return getBounds().inside(x, y); 
    }
};
