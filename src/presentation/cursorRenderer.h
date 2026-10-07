#pragma once

#include "ofMain.h"

enum class CursorState { Default, Hover, Move, Resize, Crosshair, Point };

class CursorRenderer {
public:
    void setup();
    void exit();
    void draw(CursorState state, const glm::vec2 & position) const;

private:
    void drawArrow(const glm::vec2 & tip) const;
    void drawHover(const glm::vec2 & tip) const;
    void drawMove(const glm::vec2 & center) const;
    void drawResize(const glm::vec2 & center) const;
    void drawCrosshair(const glm::vec2 & center) const;
    void drawPoint(const glm::vec2 & center) const;
};
