#include "presentation/sdfRenderer.h"

void SdfRenderer::setup() {
    if (!shader.load("shaders/raymarch.vert", "shaders/raymarch.frag")) {
        ofLogError("SdfRenderer") << "failed to load bin/data/shaders/raymarch.vert and raymarch.frag";
    }
}

void SdfRenderer::draw(const SceneCamera & camera, const ofRectangle & canvas, const SdfOptions & options) {
    if (!shader.isLoaded()) return;

    const ofRectangle viewport = ofGetNativeViewport();

    shader.begin();
    shader.setUniformMatrix4f("inverseViewProjection", camera.getInverseViewProjection());
    shader.setUniform2f("viewportSize", viewport.width, viewport.height);
    shader.setUniform3f("center", camera.getTarget());
    shader.setUniform1f("size", options.size);
    shader.setUniform1f("blend", options.blend);
    shader.setUniform1f("time", options.animate ? ofGetElapsedTimef() : 0.0f);

    ofPushStyle();
    ofDisableDepthTest();
    ofFill();
    ofSetColor(255);
    ofDrawRectangle(canvas);
    ofPopStyle();

    shader.end();
}
