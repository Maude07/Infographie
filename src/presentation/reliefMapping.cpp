#include "reliefMapping.h"

namespace {

	float smooth01(float edge0, float edge1, float x) {
		float t = ofClamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}

	float brickHeight(float u, float v, float columns, float rows, float mortar) {
		const float row = floorf(v * rows);
		const float offset = (static_cast<int>(row) % 2 == 0) ? 0.0f : 0.5f;
		const float along = u * columns + offset;
		const float fu = along - floorf(along);
		const float fv = v * rows - row;
		const float edgeU = smooth01(0.0f, mortar, std::min(fu, 1.0f - fu));
		const float edgeV = smooth01(0.0f, mortar, std::min(fv, 1.0f - fv));
		return edgeU * edgeV;
	}

	ofTexture makeNormalMap(int columns, int rows, float mortar) {
		constexpr int resolution = 128;
		constexpr float slope = 2.0f;
		constexpr float facing = 0.35f;

		vector<float> height(resolution * resolution);
		for (int y = 0; y < resolution; ++y) {
			for (int x = 0; x < resolution; ++x) {
				const float u = (x + 0.5f) / resolution;
				const float v = (y + 0.5f) / resolution;
				height[y * resolution + x] = brickHeight(u, v, columns, rows, mortar);
			}
		}

		ofPixels pixels;
		pixels.allocate(resolution, resolution, OF_PIXELS_RGB);
		auto sample = [&](int x, int y) {
			const int wrappedX = (x + resolution) % resolution;
			const int wrappedY = (y + resolution) % resolution;
			return height[wrappedY * resolution + wrappedX];
		};

		for (int y = 0; y < resolution; ++y) {
			for (int x = 0; x < resolution; ++x) {
				const float dx = sample(x - 1, y) - sample(x + 1, y);
				const float dy = sample(x, y - 1) - sample(x, y + 1);
				glm::vec3 normal = glm::normalize(glm::vec3(dx * slope, dy * slope, facing));
				pixels.setColor(x, y, ofFloatColor(
					normal.x * 0.5f + 0.5f,
					normal.y * 0.5f + 0.5f,
					normal.z * 0.5f + 0.5f));
			}
		}

		ofTexture texture;
		texture.loadData(pixels);
		texture.setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
		texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
		return texture;
	}

}

namespace reliefMapping {

	ofShader * shader() {
		static ofShader program;
		static bool attempted = false;
		if (!attempted) {
			attempted = true;
			if (!program.load("shaders/relief.vert", "shaders/relief.frag")) {
				ofLogError("reliefMapping") << "echec du chargement de bin/data/shaders/relief.vert et relief.frag";
			}
		}
		return program.isLoaded() ? &program : nullptr;
	}

	const ofTexture & normalMap(int columns, int rows, float mortar) {
		struct Key {
			int columns;
			int rows;
			int mortar;
			bool operator<(const Key & other) const {
				if (columns != other.columns) return columns < other.columns;
				if (rows != other.rows) return rows < other.rows;
				return mortar < other.mortar;
			}
		};

		const Key key {
			std::clamp(columns, 1, 32),
			std::clamp(rows, 1, 32),
			static_cast<int>(roundf(ofClamp(mortar, 0.02f, 0.45f) * 100.0f))
		};

		static map<Key, ofTexture> cache;
		auto found = cache.find(key);
		if (found == cache.end()) {
			found = cache.emplace(key, makeNormalMap(key.columns, key.rows, key.mortar / 100.0f)).first;
		}
		return found->second;
	}

	void setMaterialUniforms(ofShader & program,
		const ofFloatColor & base,
		const ofFloatColor & specular,
		float shininess,
		bool useAlbedo,
		int columns,
		int rows,
		float mortar) {
		program.setUniform4f("baseColor", base);
		program.setUniform3f("specularColor", specular.r, specular.g, specular.b);
		program.setUniform1f("shininess", shininess);
		program.setUniform1f("useTexture", useAlbedo ? 1.0f : 0.0f);
		program.setUniformTexture("normalTex", normalMap(columns, rows, mortar), 1);
	}

}
