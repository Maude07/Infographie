#pragma once

#include "ofMain.h"
#include "../../scene/sceneObject.h"
#include "../../camera/sceneCamera.h"

class SceneObject3d : public SceneObject {
public:
    float rotationY = 0.0f;
    float depth = 0.0f;
    int materialIndex = 0;
    const SceneCamera * camera = nullptr;

    static constexpr int defaultCelBands = 3;
    static constexpr float defaultCelAmbient = 0.2f;
    static constexpr float defaultCelOutline = 0.0f;

    bool celShaded = false;
    int celBands = defaultCelBands;
    float celAmbient = defaultCelAmbient;
    float celOutline = defaultCelOutline;
    static constexpr int defaultReliefColumns = 4;
    static constexpr int defaultReliefRows = 8;
    static constexpr float defaultReliefMortar = 0.12f;

    bool relief = false;
    int reliefColumns = defaultReliefColumns;
    int reliefRows = defaultReliefRows;
    float reliefMortar = defaultReliefMortar;

    bool isSquare() const override { return true; }

    void setBounds(const ofRectangle & bounds) override {
        float side = max(bounds.width, bounds.height);
        ofRectangle square;
        square.setFromCenter(bounds.getCenter(), side, side);
        SceneObject::setBounds(square);
    }

    using SceneObject::toLocal;
    glm::vec2 toLocal(const glm::vec2 & screen) const override {
        if (!camera) return screen;
        return camera->screenToPlane(screen, depth);
    }

    glm::vec3 getCenter3d() const {
        glm::vec2 c = position + size * 0.5f;
        return { c.x, c.y, depth };
    }
};
