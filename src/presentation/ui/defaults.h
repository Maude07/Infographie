#pragma once

#include "ofMain.h"

namespace defaults {
    const ofColor defaultBackgroundColor(31);
    const ofColor defaultStrokeColor(255);
    const ofColor defaultFillColor(210, 43, 43);
    constexpr float defaultStrokeWeight = 4.0f;
    const std::string defaultText = "ift3100";
    constexpr float defaultModelSize = 150.0f;

    constexpr float minStretchFactor = 1.0f;
    constexpr float maxStretchFactor = 3.0f;
    constexpr float defaultStretchFactor = 1.5f;
    constexpr float defaultMetalness = 0.7f;
    const ofColor defaultTintColor(255, 140, 90);

    inline std::vector<ofColor> defaultPalette() {
        return {
            ofColor(15, 15, 15),
            ofColor(255, 182, 193),
            ofColor(143, 131, 216),
            ofColor(155, 184, 237)
        };
    }
}
