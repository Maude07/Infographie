#include "scenePrimitive3d.h"
#include "proceduralTexture.h"
#include "presentation/celShading.h"
#include "presentation/reliefMapping.h"
#include "../../lighting/materialLibrary.h"

namespace {

    constexpr float viewTilt = 20.0f;
}

glm::mat4 ScenePrimitive3d::localTransform() const {
    const float side = min(size.x, size.y);
    return glm::translate(getCenter3d())
        * glm::rotate(glm::radians(-viewTilt), glm::vec3(1, 0, 0))
        * glm::rotate(glm::radians(rotationY), glm::vec3(0, 1, 0))
        * glm::scale(glm::vec3(side, -side, side));
}

ofRectangle ScenePrimitive3d::getScreenBounds() const {
    const float side = min(size.x, size.y);
    if (!camera || side <= 0.0f) return getBounds();

    const glm::mat4 transform = localTransform();
    ofRectangle bounds;
    bool first = true;
    for (const glm::vec3 & vertex : mesh().getVertices()) {
        glm::vec3 world = transform * glm::vec4(vertex, 1.0f);
        glm::vec3 screen = camera->worldToScreen(world);
        if (first) { bounds.set(screen.x, screen.y, 0, 0); first = false; }
        else bounds.growToInclude(screen.x, screen.y);
    }
    return bounds;
}

void ScenePrimitive3d::draw() const {
    const glm::vec2 center = position + size * 0.5f;
    const float side = min(size.x, size.y);
    if (side <= 0.0f) return;

    const bool customMaterial = materialLibrary::isCustom(materialIndex);

    static ofMaterial material;
    if (customMaterial) {
        materialLibrary::apply(material, materialLibrary::get(materialIndex));
    } else {
        materialLibrary::apply(material, materialLibrary::get(materialLibrary::defaultIndex));
        material.setDiffuseColor(color);
        material.setAmbientColor(color * 0.3f);
    }

    ofPushStyle();
    if (customMaterial) {
        ofDisableAlphaBlending();
        ofSetColor(255);
    }

    ofPushMatrix();
    ofMultMatrix(localTransform());
    ofEnableDepthTest();

    const ofTexture * tex = customMaterial ? nullptr : texture();
    ofShader * reliefShader = (relief && mesh().hasTexCoords()) ? reliefMapping::shader() : nullptr;
    ofShader * shader = celShaded ? celShading::shader() : nullptr;

    if (reliefShader) {
        const glm::vec3 fallbackLight(center.x - side, center.y - side, depth + side * 3.0f);
        const ofFloatColor base = tex ? ofFloatColor(1.0f, 1.0f, 1.0f, 1.0f)
            : (customMaterial ? materialLibrary::get(materialIndex).diffuse : ofFloatColor(color));
        const MaterialPreset & preset = materialLibrary::get(
            customMaterial ? materialIndex : materialLibrary::defaultIndex);
        ofFloatColor specular = preset.specular;
        float shininess = preset.shininess;
        if (!customMaterial && specular.r + specular.g + specular.b < 0.01f) {
            specular = ofFloatColor(0.25f);
            shininess = 32.0f;
        }

        ofDisableLighting();
        reliefShader->begin();
        celShading::setLightUniforms(*reliefShader, fallbackLight);
        reliefMapping::setMaterialUniforms(*reliefShader, base, specular, shininess, tex != nullptr,
            reliefColumns, reliefRows, reliefMortar);
        reliefShader->setUniform1f("hasNormals", 1.0f);
        if (tex) reliefShader->setUniformTexture("colorTex", *tex, 0);
        mesh().draw();
        reliefShader->end();
        ofEnableLighting();
    } else if (shader) {
        const glm::vec3 fallbackLight(center.x - side, center.y - side, depth + side * 3.0f);
        const ofFloatColor base = tex ? ofFloatColor(1.0f, 1.0f, 1.0f, 1.0f)
            : (customMaterial ? materialLibrary::get(materialIndex).diffuse : ofFloatColor(color));

        ofDisableLighting();
        shader->begin();
        celShading::setLightUniforms(*shader, fallbackLight);
        shader->setUniform4f("baseColor", base);
        shader->setUniform1f("bands", static_cast<float>(max(celBands, 2)));
        shader->setUniform1f("ambient", celAmbient);
        shader->setUniform1f("outline", celOutline);
        shader->setUniform1f("useTexture", tex ? 1.0f : 0.0f);
        shader->setUniform1f("useVertexColor", 0.0f);
        shader->setUniform1f("hasNormals", 1.0f);
        if (tex) shader->setUniformTexture("colorTex", *tex, 0);
        mesh().draw();
        shader->end();
        ofEnableLighting();
    } else {
        ofEnableLighting();
        material.begin();
        if (tex) tex->bind();
        mesh().draw();
        if (tex) tex->unbind();
        material.end();
        ofDisableLighting();
    }

    ofDisableDepthTest();
    ofPopMatrix();
    ofPopStyle();
}

const ofTexture * ScenePrimitive3d::texture() const {
    if (!checkered || !mesh().hasTexCoords()) return nullptr;
    if (!checkerReady || checkerColor != color || checkerTextureFilter != textureFilter) {
        proceduralTexture::loadCheckerboard(checkerTexture, color, textureFilter);
        checkerColor = color;
        checkerTextureFilter = textureFilter;
        checkerReady = true;
    }
    return &checkerTexture;
}

void ScenePrimitive3d::addTexCoords(ofMesh & mesh, const glm::vec2 & a, const glm::vec2 & b, const glm::vec2 & c) {
    mesh.addTexCoord(a);
    mesh.addTexCoord(b);
    mesh.addTexCoord(c);
}

void ScenePrimitive3d::addTriangle(ofMesh & mesh,
    const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c,
    const glm::vec3 & na, const glm::vec3 & nb, const glm::vec3 & nc) {
        mesh.addVertex(a);
        mesh.addNormal(na);
        mesh.addVertex(b);
        mesh.addNormal(nb);
        mesh.addVertex(c);
        mesh.addNormal(nc);
}

glm::vec3 ScenePrimitive3d::circlePoint(int segment) {
    float phi = TWO_PI * segment / segments;
    return glm::vec3(cos(phi), 0.0f, sin(phi));
}

void ScenePrimitive3d::addCap(ofMesh & mesh, float y, const glm::vec3 & normal) {
    glm::vec3 center(0.0f, y, 0.0f);
    for (int segment = 0; segment < segments; ++ segment) {
        glm::vec3 point0 = circlePoint(segment) * half + center;
        glm::vec3 point1 = circlePoint(segment + 1) * half + center;
        addTriangle(mesh, center, point0, point1, normal, normal, normal);

        auto planar = [](const glm::vec3 & p) { return glm::vec2(0.5f + p.x, 0.5f + p.z); };
        addTexCoords(mesh, planar(center), planar(point0), planar(point1));
    }
}
