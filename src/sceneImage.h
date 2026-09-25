#pragma once
#include "sceneObject.h"

using namespace std;

class SceneImage : public SceneObject {
public:
    static unique_ptr<SceneImage> load(const string & path, float maxDimension = 400.0f);
    
    void draw() const override;

private:
    ofImage image;
};
