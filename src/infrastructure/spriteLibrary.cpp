#include "infrastructure/spriteLibrary.h"
#include <regex>

namespace {
	const vector<SpriteAnimation> builtInAnimations = {
		{ "balle rebondissante", "sprites/balle.png", 64, 64, 4, 0, 8, 12.0f },
		{ "piece qui tourne", "sprites/piece.png", 64, 64, 4, 0, 8, 10.0f },
	};

	constexpr float generatedFps = 12.0f;

	void prepare(ofImage & sheet) {
		sheet.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	}
}

void SpriteLibrary::setup() {
	for (const SpriteAnimation & animation : builtInAnimations) {
		if (load(animation)) catalog.push_back(animation);
	}
	loadGenerated();
}

bool SpriteLibrary::load(const SpriteAnimation & animation) {
	if (sheets.count(animation.sheetPath) > 0) return true;

	auto sheet = make_shared<ofImage>();
	if (!sheet->load(animation.sheetPath)) {
		ofLogError() << "echec du chargement de la feuille de sprites: " << animation.sheetPath;
		return false;
	}
	prepare(*sheet);
	sheets[animation.sheetPath] = sheet;
	return true;
}

void SpriteLibrary::loadGenerated() {
	ofDirectory folder(generatedFolder);
	if (!folder.exists()) return;
	folder.allowExt("png");
	folder.listDir();
	folder.sort();

	const regex pattern(R"(^(.+)_(\d+)px_(\d+)f_.*\.png$)");
	for (const ofFile & file : folder.getFiles()) {
		smatch match;
		const string fileName = file.getFileName();
		if (!regex_match(fileName, match, pattern)) continue;

		SpriteAnimation animation;
		animation.name = match[1].str() + " (rotation)";
		animation.sheetPath = string(generatedFolder) + "/" + fileName;
		animation.frameWidth = animation.frameHeight = stoi(match[2].str());
		animation.frameCount = stoi(match[3].str());
		animation.fps = generatedFps;

		if (!load(animation)) continue;
		animation.columns = max(1, static_cast<int>(sheets[animation.sheetPath]->getWidth()) / animation.frameWidth);
		catalog.push_back(animation);
	}
}

size_t SpriteLibrary::add(const SpriteAnimation & animation, shared_ptr<ofImage> sheet) {
	prepare(*sheet);
	sheets[animation.sheetPath] = move(sheet);
	catalog.push_back(animation);
	return catalog.size() - 1;
}

shared_ptr<ofImage> SpriteLibrary::sheetFor(const SpriteAnimation & animation) const {
	auto it = sheets.find(animation.sheetPath);
	return it != sheets.end() ? it->second : nullptr;
}
