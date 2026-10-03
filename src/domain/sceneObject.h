#pragma once
#include "ofMain.h"

class SceneObject {
public:
    glm::vec2 position;
    glm::vec2 size;

	bool isDeleted = false;

    virtual ~SceneObject() = default;
    virtual void draw() const = 0;

    virtual ofRectangle getBounds() const { 
        return ofRectangle(position.x, position.y, size.x, size.y); 
    }

    virtual bool contains(float x, float y) const { 
        return getBounds().inside(x, y); 
    }
};
