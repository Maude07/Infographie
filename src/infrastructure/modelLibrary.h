#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

using namespace std;

class ModelLibrary {
public:
	shared_ptr<ofxAssimpModelLoader> load(const string & id, const string & path);
	shared_ptr<ofxAssimpModelLoader> get(const string & id) const;
	vector<string> ids() const;

private:
	map<string, shared_ptr<ofxAssimpModelLoader>> models;
};
