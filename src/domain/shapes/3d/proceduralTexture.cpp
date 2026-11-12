#include "proceduralTexture.h"

namespace {
    constexpr int checkerResolution = 64;
    constexpr int checkerCells = 8;
    constexpr float darkFactor = 60.0f / 255.0f;
}

namespace proceduralTexture {

    ofPixels checkerboardPixels(int resolution, int cellsPerSide, const ofColor & light, const ofColor & dark) {
        ofPixels pixels;
        pixels.allocate(resolution, resolution, OF_PIXELS_RGB);

        const int cellSize = max(1, resolution / cellsPerSide);
        for (int y = 0; y < resolution; ++y) {
            for (int x = 0; x < resolution; ++x) {
                bool isLight = ((x / cellSize) + (y / cellSize)) % 2 == 0;
                pixels.setColor(x, y, isLight ? light : dark);
            }
        }
        return pixels;
    }

    void loadCheckerboard(ofTexture & texture, const ofColor & baseColor) {
        ofColor dark(baseColor.r * darkFactor, baseColor.g * darkFactor, baseColor.b * darkFactor);
        texture.loadData(checkerboardPixels(checkerResolution, checkerCells, baseColor, dark));
        texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    }
}
