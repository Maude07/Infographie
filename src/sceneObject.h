#pragma once
#include "ofMain.h"

class SceneObject {
public:
    glm::vec2 position;
    glm::vec2 size;

    virtual ~SceneObject() = default;
    virtual void draw() const = 0;

    ofRectangle getBounds() const { 
        return ofRectangle(position.x, position.y, size.x, size.y); 
    }

    //TODO: a redefinir pour une detection plus precise (cercle, ligne, etc.)
    virtual bool contains(float x, float y) const { 
        return getBounds().inside(x, y); 
    }
};
