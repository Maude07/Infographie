#include "proceduralTexture.h"

namespace {
    constexpr int checkerResolution = 64;
    constexpr int checkerCells = 8;
    constexpr float darkFactor = 60.0f / 255.0f;

    constexpr int blurPassCount = 3;
    constexpr float embossBrightnessOffset = 128.0f;
    constexpr int kernelSize = 3;
    constexpr int kernelRadius = 1;

    using Kernel = std::array<float, kernelSize * kernelSize>;

    constexpr Kernel blurKernel =     { 1/16.f, 2/16.f, 1/16.f,
                                        2/16.f, 4/16.f, 2/16.f,
                                        1/16.f, 2/16.f, 1/16.f };

    constexpr Kernel sharpenKernel =  {  0, -1,  0,
                                        -1,  5, -1,
                                         0, -1,  0 };

    constexpr Kernel edgeKernel =     { -1, -1, -1,
                                        -1,  8, -1,
                                        -1, -1, -1 };

    constexpr Kernel embossKernel =   { -2, -1,  0,
                                        -1,  1,  1,
                                         0,  1,  2 };

    int wrapCoordinate(int coordinate, int length) {
        return(coordinate + length) % length;
    }

    ofPixels convolve(const ofPixels & sourcePixels, const Kernel & kernel, float brightnessOffset = 0.0f) {
        const int width = sourcePixels.getWidth();
        const int height = sourcePixels.getHeight();

        ofPixels filteredPixels;
        filteredPixels.allocate(width, height, sourcePixels.getPixelFormat());

        for (int pixelY = 0; pixelY < height; ++pixelY) {
            for (int pixelX = 0; pixelX < width; ++pixelX) {
                float red = brightnessOffset;
                float green = brightnessOffset;
                float blue = brightnessOffset;

                for (int kernelOffsetY = -kernelRadius; kernelOffsetY <= kernelRadius; ++kernelOffsetY) {
                    for (int kernelOffsetX = -kernelRadius; kernelOffsetX <= kernelRadius; ++kernelOffsetX) {
                        const int sampleX = wrapCoordinate(pixelX + kernelOffsetX, width);
                        const int sampleY = wrapCoordinate(pixelY + kernelOffsetY, height);
                        const int kernelIndex = (kernelOffsetY + kernelRadius) * kernelSize + (kernelOffsetX + kernelRadius);
                        const float weight = kernel[kernelIndex];
                        const ofColor sampleColor = sourcePixels.getColor(sampleX, sampleY);

                        red += sampleColor.r * weight;
                        green += sampleColor.g * weight;
                        blue += sampleColor.b * weight;
                    }
                }

                filteredPixels.setColor(pixelX, pixelY, ofColor(
                    ofClamp(red, 0, 255),
                    ofClamp(green, 0, 255),
                    ofClamp(blue, 0, 255)));
            }
        }
        return filteredPixels;
    }
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

    ofPixels applyFilter(const ofPixels & sourcePixels, TextureFilter filter) {
        switch (filter) {
            case TextureFilter::Blur: {
                ofPixels blurredPixels = sourcePixels;
                for (int passIndex = 0; passIndex < blurPassCount; ++passIndex) {
                    blurredPixels = convolve(blurredPixels, blurKernel);
                }
                return blurredPixels;
            }
            case TextureFilter::Sharpen:    return convolve(sourcePixels, sharpenKernel);
            case TextureFilter::EdgeDetect: return convolve(sourcePixels, edgeKernel);
            case TextureFilter::Emboss:     return convolve(sourcePixels, embossKernel, embossBrightnessOffset);
            default:                        return sourcePixels;
        }
    }

    string filterName(TextureFilter filter) {
        switch (filter) {
            case TextureFilter::Blur :      return "flou gaussien";
            case TextureFilter::Sharpen:    return "nettete";
            case TextureFilter::EdgeDetect: return "detection de contours";
            case TextureFilter::Emboss:     return "relief";
            default:                        return "aucun";
        }
    }

    void loadCheckerboard(ofTexture & texture, const ofColor & baseColor, TextureFilter filter) {
        ofColor darkColor(baseColor.r * darkFactor, baseColor.g * darkFactor, baseColor.b * darkFactor);
        ofPixels checkerPixels = checkerboardPixels(checkerResolution, checkerCells, baseColor, darkColor);
        texture.loadData(applyFilter(checkerPixels, filter));
        texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    }
}
