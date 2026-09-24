#include "ofMain.h"
#include "sceneImage.h"

using namespace std;

unique_ptr<SceneImage> SceneImage::load(const string & path, float maxDimension) {
    auto sceneImage = make_unique<SceneImage>();

    if (!sceneImage->image.load(path)) {
        ofLogError() << "echec du chargement de l'image: " << path;
        return nullptr;
    }

    float w = sceneImage->image.getWidth();
    float h = sceneImage->image.getHeight();
    float scale = min(1.0f, maxDimension / max(w, h));
    sceneImage->size = { w * scale, h * scale };

    return sceneImage;
}

void SceneImage::draw() const {
    ofPushStyle();
    ofSetColor(255);
    image.draw(position.x, position.y, size.x, size.y);
    ofPopStyle();
}