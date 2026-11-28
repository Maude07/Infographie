#include "presentation/rayTracer.h"
#include "domain/shapes/3d/scenePrimitiveSphere.h"
#include "domain/shapes/3d/scenePrimitiveCube.h"
#include "domain/lighting/materialLibrary.h"

namespace {
    constexpr int shapeSphere = 0;
    constexpr int shapeBox = 1;

    constexpr float floorMargin = 2.0f;
}

void RayTracer::setup() {
    if (!shader.load("shaders/raytrace.vert", "shaders/raytrace.frag")) {
        ofLogError("RayTracer") << "failed to load bin/data/shaders/raytrace.vert and raytrace.frag";
    }
}

void RayTracer::draw(const Scene & scene, const SceneCamera & camera, const ofRectangle & canvas,
    const RayTracingOptions & options) {
        if (!shader.isLoaded()) return;

        const ofRectangle viewport = ofGetNativeViewport();

        shader.begin();
        shader.setUniformMatrix4f("inverseViewProjection", camera.getInverseViewProjection());
        shader.setUniform2f("viewportSize", viewport.width, viewport.height);
        shader.setUniform1i("shadowsEnabled", options.shadows ? 1 : 0);
        shader.setUniform1i("floorEnabled", options.floor ? 1 : 0);
        shader.setUniform1f("floorY", uploadObjects(scene));
        uploadLights(scene.lighting);

        ofPushStyle();
        ofDisableDepthTest();
        ofFill();
        ofSetColor(255);
        ofDrawRectangle(canvas);
        ofPopStyle();

        shader.end();
}

float RayTracer::uploadObjects(const Scene & scene) {
    vector<int> types;
    vector<glm::mat4> worldToLocal;
    vector<glm::vec3> colors;
    float lowestPoint = 0.0f;

    for (const auto & object : scene.getObjects()) {
        if (static_cast<int>(types.size()) >= maxObjects) break;
        if (!scene.isVisible(object.get())) continue;

        const auto * primitive = dynamic_cast<const ScenePrimitive3d *>(object.get());
        if (!primitive) continue;
        const float side = min(primitive->size.x, primitive->size.y);
        if (side <= 0.0f) continue;

        const glm::mat4 localToWorld = primitive->localTransform();
        float bottom;
        if (dynamic_cast<const ScenePrimitiveSphere *>(primitive)) {
            types.push_back(shapeSphere);
            bottom = primitive->getCenter3d().y + side * 0.5f;
        } else if (dynamic_cast<const ScenePrimitiveCube *>(primitive)) {
            types.push_back(shapeBox);
            bottom = -numeric_limits<float>::max();
            for (float x : { -0.5f, 0.5f })
                for (float y : { -0.5f, 0.5f })
                    for (float z : { -0.5f, 0.5f })
                        bottom = max(bottom, (localToWorld * glm::vec4(x, y, z, 1.0f)).y);
        } else {
            continue;
        }

        const ofFloatColor color = materialLibrary::isCustom(primitive->materialIndex)
            ? materialLibrary::get(primitive->materialIndex).diffuse
            : ofFloatColor(primitive->color);

        worldToLocal.push_back(glm::inverse(localToWorld));
        colors.push_back(glm::vec3(color.r, color.g, color.b));
        lowestPoint = types.size() == 1 ? bottom : max(lowestPoint, bottom);
    }
    
    const int count = static_cast<int>(types.size());
    shader.setUniform1i("objectCount", count);
    if (count > 0) {
        shader.setUniform1iv("objectType", types.data(), count);
        shader.setUniformMatrix4f("objectWorldToLocal", worldToLocal[0], count);
        shader.setUniform3fv("objectColor", &colors[0].x, count);
    }
    return lowestPoint + floorMargin;
}

void RayTracer::uploadLights(const LightRig & rig) {
    vector<int> types;
    vector<glm::vec3> positions, directions, colors, attenuations;
    vector<float> spotCosCutoffs, spotExponents;

    for (const SceneLight & light : rig.lights) {
        if (!light.enabled) continue;
        if (static_cast<int>(types.size()) >= maxLights) break;

        glm::vec3 direction = light.target - light.position;
        direction = glm::dot(direction, direction) < 1e-6f
            ? glm::vec3(0.0f, 0.0f, -1.0f)
            : glm::normalize(direction);

        const ofFloatColor color(light.color);
        types.push_back(static_cast<int>(light.type));
        positions.push_back(light.position);
        directions.push_back(direction);
        colors.push_back(glm::vec3(color.r, color.g, color.b));
        attenuations.push_back(glm::vec3(light.constant, light.linear, light.quadratic));
        spotCosCutoffs.push_back(std::cos(glm::radians(light.spotAngle)));
        spotExponents.push_back(light.spotConcentration);
    }

    const int count = static_cast<int>(types.size());
    shader.setUniform1i("lightCount", count);
    if (count == 0) return;
    shader.setUniform1iv("lightType", types.data(), count);
    shader.setUniform3fv("lightPosition", &positions[0].x, count);
    shader.setUniform3fv("lightDirection", &directions[0].x, count);
    shader.setUniform3fv("lightColor", &colors[0].x, count);
    shader.setUniform3fv("lightAttenuation", &attenuations[0].x, count);
    shader.setUniform1fv("lightSpotCosCutoff", spotCosCutoffs.data(), count);
    shader.setUniform1fv("lightSpotExponent", spotExponents.data(), count);
}
