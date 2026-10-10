#include "presentation/cursorRenderer.h"

using namespace std;

namespace {

    using Segment = pair<glm::vec2, glm::vec2>;

    const ofColor cursorOutlineColor(0);
    const ofColor cursorFillColor(255);
    constexpr float outlineWidth = 3.0f;
    constexpr float strokeWidth = 1.0f;

    const vector<glm::vec2> arrowShape = {
        {0, 0}, {0, 17}, {4, 13}, {7, 19}, {10, 18}, {7, 12}, {12, 12}
    };

    const glm::vec2 hoverBadgeOffset(13, 15);
    constexpr float hoverBadgeSize = 7.0f;

    constexpr float doubleArrowLength = 10.0f;
    constexpr float arrowHeadSize = 4.0f;

    constexpr float crosshairSize = 10.0f;
    constexpr float crosshairGap = 3.0f;

    constexpr float pointRingRadius = 6.0f;
    constexpr float pointDotRadius = 1.5f;

    void drawPolygon(const vector<glm::vec2> & points, const glm::vec2 & origin) {
        ofFill();
        ofSetColor(cursorFillColor);
        ofBeginShape();
        for (const auto & point : points) ofVertex(origin + point);
        ofEndShape(true);

        ofNoFill();
        ofSetColor(cursorOutlineColor);
        ofSetLineWidth(strokeWidth);
        ofBeginShape();
        for (const auto & point : points) ofVertex(origin + point);
        ofEndShape(true);
    }

    void drawOutlineSegments(const vector<Segment> & segments) {
        ofSetLineWidth(outlineWidth);
        ofSetColor(cursorOutlineColor);
        for (const auto & segment : segments) ofDrawLine(segment.first, segment.second);

        ofSetLineWidth(strokeWidth);
        ofSetColor(cursorFillColor);
        for (const auto & segment : segments) ofDrawLine(segment.first, segment.second);
    }

    void appendDoubleArrow(vector<Segment> & segments, const glm::vec2 & center, const glm::vec2 & direction) {
        glm::vec2 dir = glm::normalize(direction);
        glm::vec2 perp(-dir.y, dir.x);

        glm::vec2 a = center - dir * doubleArrowLength;
        glm::vec2 b = center + dir * doubleArrowLength;
        segments.push_back({ a, b });

        for (auto [tip, back] : { pair{ a, dir }, pair{ b, -dir } }) {
            glm::vec2 base = tip + back * arrowHeadSize;
            segments.push_back({ tip, base + perp * arrowHeadSize });
            segments.push_back({ tip, base - perp * arrowHeadSize });
        }
    }
}

void CursorRenderer::setup() {
    ofHideCursor();
}

void CursorRenderer::exit() {
    ofShowCursor();
}

void CursorRenderer::draw(CursorState state, const glm::vec2 & position) const {
    ofPushStyle();
    switch (state) {
        case CursorState::Default: drawArrow(position); break;
        case CursorState::Hover: drawHover(position); break;
        case CursorState::Move: drawMove(position); break;
        case CursorState::Resize: drawResize(position); break;
        case CursorState::Crosshair: drawCrosshair(position); break;
        case CursorState::Point: drawPoint(position); break;
    }
    ofPopStyle();
}

void CursorRenderer::drawArrow(const glm::vec2 & tip) const {
    drawPolygon(arrowShape, tip);
}

void CursorRenderer::drawHover(const glm::vec2 & tip) const {
    drawArrow(tip);

    ofRectangle badge(tip + hoverBadgeOffset, hoverBadgeSize, hoverBadgeSize);
    ofFill();
    ofSetColor(cursorFillColor);
    ofDrawRectangle(badge);
    ofNoFill();
    ofSetColor(cursorOutlineColor);
    ofSetLineWidth(strokeWidth);
    ofDrawRectangle(badge);
}

void CursorRenderer::drawMove(const glm::vec2 & center) const {
    vector<Segment> segments;
    appendDoubleArrow(segments, center, { 1, 0 });
    appendDoubleArrow(segments, center, { 0, 1 });
    drawOutlineSegments(segments);
}

void CursorRenderer::drawResize(const glm::vec2 & center) const {
    vector<Segment> segments;
    appendDoubleArrow(segments, center, { 1, 1 });
    drawOutlineSegments(segments);
}

void CursorRenderer::drawCrosshair(const glm::vec2 & center) const {
    drawOutlineSegments({
        { center + glm::vec2(-crosshairSize, 0), center + glm::vec2(-crosshairGap, 0) },
        { center + glm::vec2(crosshairGap, 0), center + glm::vec2(crosshairSize, 0) },
        { center + glm::vec2(0, -crosshairSize), center + glm::vec2(0, -crosshairGap) },
        { center + glm::vec2(0, crosshairGap), center + glm::vec2(0, crosshairSize) },
    });
}

void CursorRenderer::drawPoint(const glm::vec2 & center) const {
    ofNoFill();
    ofSetLineWidth(outlineWidth);
    ofSetColor(cursorOutlineColor);
    ofDrawCircle(center, pointRingRadius);
    ofSetLineWidth(strokeWidth);
    ofSetColor(cursorFillColor);
    ofDrawCircle(center, pointRingRadius);

    ofFill();
    ofDrawCircle(center, pointDotRadius);
}
