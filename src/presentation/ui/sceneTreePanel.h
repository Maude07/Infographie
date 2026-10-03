#pragma once

#include "ofMain.h"
#include "domain/scene.h"

class SceneTreePanel {
public: 
    void draw(const Scene & scene, const SceneObject * selection, const glm::vec2 & mouse) const;
    SceneObject * hitTest(const Scene & scene, float x, float y) const;

private:
    static constexpr float rowWidth = 200.0f;
    static constexpr float rowHeight = 30.0f;
    static constexpr float rowGap = 2.0f;
    static constexpr float marginRight = 20.0f;
    static constexpr float marginTop = 60.0f;

    ofRectangle getRowBounds(size_t index) const;
};
