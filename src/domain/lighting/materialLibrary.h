#pragma once

#include "ofMain.h"
#include <string>
#include <vector>

struct MaterialPreset {
	std::string name;
	ofFloatColor ambient;
	ofFloatColor diffuse;
	ofFloatColor specular;
	ofFloatColor emissive;
	float shininess;
};

namespace materialLibrary {

	constexpr int defaultIndex = 0;

	inline const std::vector<MaterialPreset> & presets() {
		static const std::vector<MaterialPreset> list = {
			{ "par defaut", { 0.2f, 0.2f, 0.2f }, { 0.8f, 0.8f, 0.8f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.2f },
			{ "or", { 0.30f, 0.22f, 0.06f }, { 0.45f, 0.33f, 0.08f }, { 1.0f, 0.86f, 0.45f }, { 0.04f, 0.03f, 0.0f }, 110.0f },
			{ "argent", { 0.22f, 0.22f, 0.24f }, { 0.32f, 0.32f, 0.34f }, { 1.0f, 1.0f, 1.0f }, { 0.03f, 0.03f, 0.035f }, 140.0f },
			{ "cuivre", { 0.26f, 0.11f, 0.05f }, { 0.48f, 0.19f, 0.08f }, { 1.0f, 0.62f, 0.45f }, { 0.03f, 0.01f, 0.0f }, 90.0f },
			{ "emeraude", { 0.0215f, 0.1745f, 0.0215f }, { 0.07568f, 0.61424f, 0.07568f }, { 0.633f, 0.727811f, 0.633f }, { 0.0f, 0.0f, 0.0f }, 76.8f },
			{ "rubis", { 0.1745f, 0.01175f, 0.01175f }, { 0.61424f, 0.04136f, 0.04136f }, { 0.727811f, 0.626959f, 0.626959f }, { 0.0f, 0.0f, 0.0f }, 76.8f },
			{ "caoutchouc noir", { 0.03f, 0.03f, 0.03f }, { 0.06f, 0.06f, 0.06f }, { 0.12f, 0.12f, 0.12f }, { 0.0f, 0.0f, 0.0f }, 6.0f },
			{ "plastique blanc", { 0.45f, 0.45f, 0.45f }, { 0.92f, 0.92f, 0.92f }, { 0.45f, 0.45f, 0.45f }, { 0.12f, 0.12f, 0.12f }, 40.0f },
			{ "neon", { 0.0f, 0.0f, 0.0f }, { 0.1f, 0.1f, 0.1f }, { 0.2f, 0.2f, 0.2f }, { 0.1f, 0.9f, 1.0f }, 8.0f }
		};
		return list;
	}

	inline int count() {
		return static_cast<int>(presets().size());
	}

	inline const MaterialPreset & get(int index) {
		return presets()[std::clamp(index, 0, count() - 1)];
	}

	inline bool & enabled() {
		static bool value = true;
		return value;
	}

	inline bool isCustom(int index) {
		return enabled() && index > defaultIndex && index < count();
	}

	inline void apply(ofMaterial & material, const MaterialPreset & preset) {
		material.setAmbientColor(preset.ambient);
		material.setDiffuseColor(preset.diffuse);
		material.setSpecularColor(preset.specular);
		material.setEmissiveColor(preset.emissive);
		material.setShininess(preset.shininess);
	}
}
