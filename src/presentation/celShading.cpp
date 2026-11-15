#include "presentation/celShading.h"

namespace celShading {

	ofShader * shader() {
		static ofShader program;
		static bool attempted = false;
		if (!attempted) {
			attempted = true;
			if (!program.load("shaders/cel.vert", "shaders/cel.frag")) {
				ofLogError("celShading") << "echec du chargement de bin/data/shaders/cel.vert et cel.frag";
			}
		}
		return program.isLoaded() ? &program : nullptr;
	}

}