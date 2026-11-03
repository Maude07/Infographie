#pragma once

#include "ofCubeMap.h"
#include <string>
#include <vector>

class SkyboxCatalog {
public:
	struct Descriptor {
		std::string dataPath;
		std::string label;
	};

	void loadFromFolder(const std::string & folder = "skybox", int faceResolution = 0);

	size_t size() const { return entries.size(); }
	bool isLoaded(size_t index) const;
	bool hasAnyLoaded() const;
	const std::string & label(size_t index) const;

	void draw(size_t index);

private:
	void loadEntry(const Descriptor & descriptor, int faceResolution);

	struct Entry {
		Descriptor descriptor;
		ofCubeMap map;
		bool loaded = false;
	};

	std::vector<Entry> entries;
};
