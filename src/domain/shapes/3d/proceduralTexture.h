#pragma once

#include "ofMain.h"

enum class TextureFilter { None = 0, Blur, Sharpen, EdgeDetect, Emboss, Count };

namespace proceduralTexture {

    ofPixels checkerboardPixels(int resolution, int cellsPerSide, const ofColor & light, const ofColor & dark);

    ofPixels applyFilter(const ofPixels & sourcePixels, TextureFilter filter);
    string filterName(TextureFilter filter);

    void loadCheckerboard(ofTexture & texture, const ofColor & baseColor, TextureFilter fitler = TextureFilter::None);
}
