#pragma once
#include "domain/shapes/3d/scenePrimitive3d.h"
#include "ofMain.h"

class ScenePrimitiveCylinder : public ScenePrimitive3d {
protected:
    const ofMesh & mesh() const override;

private:
    static ofMesh generate();
};
