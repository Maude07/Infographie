#pragma once

#include "ofMain.h"
#include "sceneObject3d.h"
#include "proceduralTexture.h"

class ScenePrimitive3d : public SceneObject3d {
public: 
    ofColor color = ofColor::white;
    bool checkered = true;
    TextureFilter textureFilter = TextureFilter::None;

    void draw() const override;

    ofRectangle getScreenBounds() const override;

protected:
    static constexpr float half = 0.5f;
    static constexpr int segments = 32;

    virtual const ofMesh & mesh() const = 0;

    const ofTexture * texture() const;

    glm::mat4 localTransform() const;

    static void addTriangle(ofMesh & mesh,
        const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
        const glm::vec3 & na, const glm::vec3 & nb, const glm::vec3 & nc);
    static glm::vec3 circlePoint(int segment);
    static void addCap(ofMesh & mesh, float y, const glm::vec3 & normal);
    static void addTexCoords(ofMesh & mesh, const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c);

    static constexpr float sphereRepeatU = 2.0f;
    static constexpr float roundRepeatU = 3.0f;

private:
    mutable ofTexture checkerTexture;
    mutable ofColor checkerColor;
    mutable TextureFilter checkerTextureFilter = TextureFilter::None;
    mutable bool checkerReady = false;
};
