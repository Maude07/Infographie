#include "sceneImage.h"

std::unique_ptr<SceneImage> SceneImage::load(const std::string & path, float maxDimension) {
    auto sceneImage = std::make_unique<SceneImage>();

    if (!sceneImage->image.load(path)) {
        ofLogError() << "echec du chargement de l'image: " << path;
        return nullptr;
    }

    float w = sceneImage->image.getWidth();
    float h = sceneImage->image.getHeight();
    float scale = std::min(1.0f, maxDimension / std::max(w, h));
    sceneImage->size = { w * scale, h * scale };

    return sceneImage;
}

void SceneImage::draw() const {
    ofPushStyle();
    ofSetColor(255);
    image.draw(position.x, position.y, size.x, size.y);
    ofPopStyle();
}