#include "presentation/depthOfField.h"

namespace {
    const string vertexShader = R"(
        #version 150
        in vec4 position;
        in vec2 texcoord;
        out vec2 texCoordVarying;
        uniform mat4 modelViewProjectionMatrix;

        void main() {
            texCoordVarying = texcoord;
            gl_Position = modelViewProjectionMatrix * position;
        }
    )";

    const string fragmentShader = R"(
        #version 150
        uniform sampler2D colorTex;
        uniform sampler2D depthTex;
        uniform vec2 texSize;
        uniform float nearClip;
        uniform float farClip;
        uniform float ortho;
        uniform float canvasDistance;
        uniform float focusDistance;
        uniform float focusRange;
        uniform float maxBlur;

        in vec2 texCoordVarying;
        out vec4 outputColor;

        const int sampleCount = 32;
        const float goldenAngle = 2.39996;

        float distanceAt(vec2 uv) {
            float depth = texture(depthTex, uv).r;
            if (depth >= 1.0) return canvasDistance;
            if (ortho > 0.5) return nearClip + depth * (farClip - nearClip);
            float ndc = depth * 2.0 - 1.0;
            return 2.0 * nearClip * farClip / (farClip + nearClip - ndc * (farClip - nearClip));
        }

        void main() {
            vec2 uv = texCoordVarying;

            float outOfFocus = abs(distanceAt(uv) - focusDistance) - focusRange * 0.5;
            float radius = clamp(outOfFocus / max(focusRange, 1.0), 0.0, 1.0) * maxBlur;
            vec2 blurScale = vec2(radius) / texSize;

            vec4 sum = texture(colorTex, uv);
            for (int i = 1; i < sampleCount; ++i) {
                float r = sqrt(float(i) / float(sampleCount));
                float angle = float(i) * goldenAngle;
                sum += texture(colorTex, uv + vec2(cos(angle), sin(angle)) * r * blurScale);
            }
            outputColor = sum / float(sampleCount);
        }
    )";

}

void DepthOfField::setup() {
    ofDisableArbTex();
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    shader.bindDefaults();
    shader.linkProgram();
    allocate(ofGetWidth(), ofGetHeight());
}

void DepthOfField::allocate(int width, int height) {
    ofFbo::Settings settings;
    settings.width = width;
    settings.height = height;
    settings.internalformat = GL_RGBA;
    settings.useDepth = true;
    settings.depthStencilAsTexture = true;
    fbo.allocate(settings);
}

void DepthOfField::begin() {
    if (fbo.getWidth() != ofGetWidth() || fbo.getHeight() != ofGetHeight()) {
        allocate(ofGetWidth(), ofGetHeight());
    }
    fbo.begin();
}

void DepthOfField::end() {
    fbo.end();
}

void DepthOfField::draw(const SceneCamera & camera, float focusDistance, float focusRange, float maxBlur) {
    shader.begin();
    shader.setUniformTexture("colorTex", fbo.getTexture(), 0);
    shader.setUniformTexture("depthTex", fbo.getDepthTexture(), 1);
    shader.setUniform2f("texSize", fbo.getWidth(), fbo.getHeight());
    shader.setUniform1f("nearClip", camera.getNearClip());
    shader.setUniform1f("farClip", camera.getFarClip());
    shader.setUniform1f("ortho", camera.isOrtho() ? 1.0f : 0.0f);
    shader.setUniform1f("canvasDistance", camera.getRadius());
    shader.setUniform1f("focusDistance", focusDistance);
    shader.setUniform1f("focusRange", focusRange);
    shader.setUniform1f("maxBlur", maxBlur);

    ofPushStyle();
    ofSetColor(255);
    fbo.draw(0, 0);
    ofPopStyle();

    shader.end();
}
