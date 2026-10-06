#pragma once

#include "domain/sceneObject.h"
#include "domain/scene.h"

class TransformTool {

public: 
    void mousePressed(Scene & scene, float x, float y);
    void mouseDragged(float x, float y);
    void mouseReleased();
    void drawOverlay() const;

    SceneObject * getSelection() const { return selection; } 
    void select(SceneObject * object) { selection = object; mode = Mode::None; }
    void clearSelection() { selection = nullptr; mode = Mode::None; }

private:
    enum class Mode { None, Move, Resize, Rotate };

    SceneObject * selection = nullptr;
    Mode mode = Mode::None;

    glm::vec2 dragOffset;
    glm::vec2 resizeAnchor;
    float aspectRatio = 1.0f;
    float rotateStartAngle = 0.0f;
    float rotateStartValue = 0.0f;

    static constexpr float handleSize = 10.0f;
    static constexpr float rotateHandleDistance = 25.0f;
    static constexpr float rotationSnap = 15.0f;

    ofRectangle getResizeHandle() const;
    ofRectangle getRotateHandle() const;

    float angleFromCenter(const glm::vec2 & point) const;
    void resizeTo(const glm::vec2 & mouse);
    void rotateTo(const glm::vec2 & mouse);
};
