#include "ofMain.h"
#include "infrastructure/skyboxCatalog.h"

namespace {

constexpr int kMinFaceResolution = 256;
constexpr int kMaxFaceResolution = 2048;
constexpr int kDefaultFaceResolution = 512;

constexpr const char * kSkyboxExtensions[] = { "png", "jpg", "jpeg", "hdr", "exr" };

int faceResolutionForImage(const std::string & dataPath, int requestedResolution) {
	if (requestedResolution > 0) {
		return ofClamp(ofNextPow2(requestedResolution), kMinFaceResolution, kMaxFaceResolution);
	}
	ofPixels pixels;
	if (!ofLoadImage(pixels, dataPath)) {
		return kDefaultFaceResolution;
	}
	return ofClamp(ofNextPow2(pixels.getHeight()), kMinFaceResolution, kMaxFaceResolution);
}

bool isSupportedExtension(std::string ext) {
	ext = ofToLower(ext);
	if (!ext.empty() && ext.front() == '.') {
		ext.erase(0, 1);
	}
	for (const char * allowed : kSkyboxExtensions) {
		if (ext == allowed) {
			return true;
		}
	}
	return false;
}

}

void SkyboxCatalog::loadFromFolder(const std::string & folder, int faceResolution) {
	entries.clear();

	ofDirectory dir(folder);
	if (!dir.exists()) {
		ofLogNotice("SkyboxCatalog: Directory missing:") << folder;
		return;
	}

	dir.listDir();
	dir.sort();

	for (int i = 0; i < dir.size(); ++i) {
		const ofFile & file = dir.getFile(i);
		if (!file.isFile()) {
			continue;
		}
		const std::string ext = ofFilePath::getFileExt(file.path());
		if (!isSupportedExtension(ext)) {
			continue;
		}
		const std::string fileName = ofFilePath::getFileName(file.path());
		Descriptor descriptor;
		descriptor.dataPath = folder + "/" + fileName;
		descriptor.label = fileName;
		loadEntry(descriptor, faceResolution);
	}

	if (entries.empty()) {
		ofLogNotice("SkyboxCatalog: No skybox images in") << folder;
	}
}

void SkyboxCatalog::loadEntry(const Descriptor & descriptor, int faceResolution) {
	entries.emplace_back();
	Entry & entry = entries.back();
	entry.descriptor = descriptor;

	const int faceRes = faceResolutionForImage(descriptor.dataPath, faceResolution);
	const bool loadOk = entry.map.load(descriptor.dataPath, faceRes, false);
	entry.loaded = loadOk && entry.map.hasCubeMap();

	if (!entry.loaded) {
		ofLogWarning("SkyboxCatalog: Failed to load") << descriptor.dataPath;
		entries.pop_back();
	}
}

bool SkyboxCatalog::isLoaded(size_t index) const {
	return index < entries.size() && entries[index].loaded;
}

bool SkyboxCatalog::hasAnyLoaded() const {
	return !entries.empty();
}

const std::string & SkyboxCatalog::label(size_t index) const {
	static const std::string empty;
	if (index >= entries.size()) return empty;
	return entries[index].descriptor.label;
}

void SkyboxCatalog::draw(size_t index) {
	if (index >= entries.size() || !entries[index].loaded) {
		return;
	}
	if (!entries[index].map.hasCubeMap()) {
		return;
	}
	entries[index].map.draw();
}
