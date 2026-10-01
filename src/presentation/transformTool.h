#pragma once

#include "sceneObject.h"
#include "scene.h"

class TransformTool {

public: 
    void mousePressed(Scene & scene, float x, float y);
    void mouseDragged(float x, float y);
    void mouseReleased();
    void drawOverlay() const;

    SceneObject * getSelection() const { return selection; } 
    void clearSelection() { selection = nullptr; }

private:
    SceneObject * selection = nullptr;
    glm::vec2 dragOffset;
    bool isResizing = false;
    float minSize = 20.0f;
    static constexpr float handleSize = 10.0f;

    ofRectangle getHandleBounds() const;
};
