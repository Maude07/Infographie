#pragma once

#include "ofMain.h"

namespace defaults {
    const ofColor defaultBackgroundColor(31);
    const ofColor defaultStrokeColor(255);
    const ofColor defaultFillColor(31);
    constexpr float defaultStrokeWeight = 4.0f;
    const std::string defaultText = "ift3100";

    std::vector<ofColor> defaultPalette() {
        return {
            ofColor(15, 15, 15),
            ofColor(255, 182, 193),
            ofColor(143, 131, 216),
            ofColor(155, 184, 237)
        };
    }
}
