#pragma once

#include "ofMain.h"

namespace reliefMapping {

	ofShader * shader();
	const ofTexture & normalMap(int columns, int rows, float mortar);

	void setMaterialUniforms(ofShader & program,
		const ofFloatColor & base,
		const ofFloatColor & specular,
		float shininess,
		bool useAlbedo,
		int columns,
		int rows,
		float mortar);

}
