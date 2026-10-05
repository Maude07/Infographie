#pragma once
#include "domain/sceneObject.h"

using namespace std;

class SceneImage : public SceneObject {
public:
    static constexpr float defaultMaxDimension = 400.0f;

    static unique_ptr<SceneImage> load(const string & path, float maxDimension = defaultMaxDimension);
    
    void draw() const override;

private:
    ofImage image;
};
