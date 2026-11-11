#pragma once
#include "ofMain.h"
#include "domain/animation/spriteAnimation.h"

using namespace std;

class SpriteLibrary {
public:
	static constexpr const char * generatedFolder = "sprites/generated";
	static constexpr const char * importedFolder = "sprites/imported";

	struct ImportSettings {
		int columns = 4;
		int rows = 1;
		int frameCount = 0;
		float fps = 12.0f;
		bool detectGrid = false;
		bool detected = false;
	};

	static constexpr int maxGridDivisions = 32;

	static string fileSafeName(const string & text);
	static bool detectGrid(const ofPixels & pixels, int & columns, int & rows, int & frameCount);

	void setup();

	size_t add(const SpriteAnimation & animation, shared_ptr<ofImage> sheet);
	bool import(const ofBuffer & data, const string & sourceName, ImportSettings & settings,
		size_t & index, string & error);

	const vector<SpriteAnimation> & animations() const { return catalog; }
	shared_ptr<ofImage> sheetFor(const SpriteAnimation & animation) const;

private:
	bool load(const SpriteAnimation & animation);
	void loadGenerated();
	void loadImported();
	int findImported(const string & name) const;
	static string importedBaseName(const string & sourceName);

	vector<SpriteAnimation> catalog;
	map<string, shared_ptr<ofImage>> sheets;
};
