#pragma once

#include "ofMain.h"

namespace proceduralTexture {

    ofPixels checkerboardPixels(int resolution, int cellsPerSide, const ofColor & light, const ofColor & dark);

    void loadCheckerboard(ofTexture & texture, const ofColor & baseColor);
}
