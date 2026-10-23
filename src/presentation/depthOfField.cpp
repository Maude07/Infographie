#include "presentation/depthOfField.h"

namespace {
    const string vertexShader= R"(
        #version 120
        void main() {
            gl_TexCoord[0] = gl_MultiTexCoord0;
            gl_Position = ftransform();
        }
    )";

    const string fragmentShader = R"(
        #version 120
        #extension GL_ARB_texture_rectangle : enable
        
        uniform sampler2DRect colorTex;
        uniform sampler2DRect depthTex;
        uniform float nearClip;
        uniform float farClip;
        uniform float ortho;
        uniform float canvasDistance;
        uniform float focusDistance;
        uniform float focusRange;
        uniform float maxBlur;

        const int sampleCount = 32;
        const float goldenAngle = 2.39996;

        float distanceAt(vec2 uv) {
            float depth = texture2DRect(depthTex, uv).r;
            if (depth >= 1.0) return canvasDistance;
            if (ortho > 0.5) return nearClip + depth * (farClip - nearClip);
            float ndc = depth * 2.0 - 1.0;
            return 2.0 * nearClip * farClip / (farClip + nearClip - ndc * (farClip - nearClip));
        }

        void main() {
            vec2 uv = gl_TexCoord[0].st;

            float outOfFocus = abs(distanceAt(uv) - focusDistance) - focusRange * 0.5;
            float radius = clamp(outOfFocus / max(focusRange, 1.0), 0.0, 1.0) * maxBlur;

            vec4 sum = texture2DRect(colorTex, uv);
            for (int i = 1; i < sampleCount; ++i) {
                float r = sqrt(float(i) / float (sampleCount)) * radius;
                float angle = float(i) * goldenAngle;
                sum += texture2DRect(colorTex, uv + vec2(cos(angle), sin(angle)) * r);
            }
            gl_FragColor = sum / float(sampleCount);
        }
    )";

}

void DepthOfField::setup() {
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
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