#include "modelLibrary.h"

shared_ptr<ofxAssimpModelLoader> ModelLibrary::load(const string& id, const string& path) {
	auto it = models.find(id);
	if (it != models.end()) return it->second;

	auto model = make_shared<ofxAssimpModelLoader>();
	if (!model->loadModel(path, false)) {
		ofLogError("ModelLibrary") << "Impossible de charger " << path;
		return nullptr;
	}
	model->setScaleNormalization(false);

	models[id] = model;
	return model;
}

shared_ptr<ofxAssimpModelLoader> ModelLibrary::get(const string& id) const {
	auto it = models.find(id);
	if (it != models.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

vector<string> ModelLibrary::ids() const {
	vector<string> result;
	for (const auto& entry : models) {
		result.push_back(entry.first);
	}
	return result;
}
