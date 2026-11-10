#include "infrastructure/spriteLibrary.h"
#include <regex>

namespace {
	const vector<SpriteAnimation> builtInAnimations = {
		{ "balle rebondissante", "sprites/balle.png", 64, 64, 4, 0, 8, 12.0f },
		{ "piece qui tourne", "sprites/piece.png", 64, 64, 4, 0, 8, 10.0f },
	};

	constexpr float generatedFps = 12.0f;
	constexpr float importedFps = 12.0f;

	void prepare(ofImage & sheet) {
		sheet.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	}

	constexpr int minCellSize = 8;
	constexpr int alphaThreshold = 10;
	constexpr int colorTolerance = 24;

	vector<bool> emptyMask(const ofPixels & source) {
		ofPixels pixels = source;
		pixels.setImageType(OF_IMAGE_COLOR_ALPHA);
		const size_t width = pixels.getWidth();
		const size_t height = pixels.getHeight();
		const unsigned char * data = pixels.getData();

		const bool transparentBackground = data[3] <= alphaThreshold;
		const int backgroundR = data[0], backgroundG = data[1], backgroundB = data[2];

		vector<bool> empty(width * height);
		for (size_t i = 0; i < width * height; ++i) {
			const unsigned char * p = data + i * 4;
			const bool transparent = p[3] <= alphaThreshold;
			const bool background = !transparentBackground
				&& abs(p[0] - backgroundR) <= colorTolerance
				&& abs(p[1] - backgroundG) <= colorTolerance
				&& abs(p[2] - backgroundB) <= colorTolerance;
			empty[i] = transparent || background;
		}
		return empty;
	}

	int bestDivisions(const vector<bool> & occupied) {
		const int length = static_cast<int>(occupied.size());
		int best = 0;
		for (int divisions = 1; divisions <= SpriteLibrary::maxGridDivisions; ++divisions) {
			if (length / divisions < minCellSize) break;

			bool valid = true;
			for (int k = 0; k < divisions && valid; ++k) {
				const int start = k * length / divisions;
				const int end = (k + 1) * length / divisions;
				if (k > 0 && (occupied[start] || occupied[start - 1])) valid = false;
				if (valid && find(occupied.begin() + start, occupied.begin() + end, true) == occupied.begin() + end) {
					valid = false;
				}
			}
			if (valid) best = divisions;
		}
		return best;
	}

	vector<ofFile> listPngFiles(const string & path) {
		ofDirectory folder(path);
		if (!folder.exists()) return {};
		folder.allowExt("png");
		folder.listDir();
		folder.sort();
		return folder.getFiles();
	}
}

string SpriteLibrary::importedBaseName(const string & sourceName) {
	string name = fileSafeName(sourceName);
	const regex gridSuffix(R"(^(.+?)-\d+x\d+-\d+f(-[0-9-]+)?$)");
	smatch match;
	while (regex_match(name, match, gridSuffix)) name = match[1].str();
	return name;
}

int SpriteLibrary::findImported(const string & name) const {
	const string prefix = string(importedFolder) + "/";
	for (size_t i = 0; i < catalog.size(); ++i) {
		if (catalog[i].name == name && catalog[i].sheetPath.rfind(prefix, 0) == 0) return static_cast<int>(i);
	}
	return -1;
}

string SpriteLibrary::fileSafeName(const string & text) {
	string out;
	for (unsigned char c : text) {
		if (isalnum(c)) out += static_cast<char>(tolower(c));
		else if (!out.empty() && out.back() != '-') out += '-';
	}
	while (!out.empty() && out.back() == '-') out.pop_back();
	return out.empty() ? "sprite" : out;
}

void SpriteLibrary::setup() {
	for (const SpriteAnimation & animation : builtInAnimations) {
		if (load(animation)) catalog.push_back(animation);
	}
	loadGenerated();
	loadImported();
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
	const regex pattern(R"(^(.+)_(\d+)px_(\d+)f_.*\.png$)");
	for (const ofFile & file : listPngFiles(generatedFolder)) {
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

void SpriteLibrary::loadImported() {
	const regex pattern(R"(^(.+)_(\d+)x(\d+)_(\d+)f(_.*)?\.png$)");
	for (const ofFile & file : listPngFiles(importedFolder)) {
		smatch match;
		const string fileName = file.getFileName();
		if (!regex_match(fileName, match, pattern)) continue;

		SpriteAnimation animation;
		animation.name = importedBaseName(match[1].str());
		animation.sheetPath = string(importedFolder) + "/" + fileName;
		animation.columns = stoi(match[2].str());
		const int rows = stoi(match[3].str());
		animation.frameCount = stoi(match[4].str());
		animation.fps = importedFps;

		if (!load(animation) || animation.columns < 1 || rows < 1) continue;
		const auto & sheet = sheets[animation.sheetPath];
		animation.frameWidth = static_cast<int>(sheet->getWidth()) / animation.columns;
		animation.frameHeight = static_cast<int>(sheet->getHeight()) / rows;

		const int existing = findImported(animation.name);
		if (existing < 0) {
			catalog.push_back(animation);
			continue;
		}
		const string supersededPath = catalog[existing].sheetPath;
		sheets.erase(supersededPath);
		ofFile::removeFile(supersededPath);
		catalog[existing] = animation;
	}
}

bool SpriteLibrary::detectGrid(const ofPixels & pixels, int & columns, int & rows, int & frameCount) {
	const int width = static_cast<int>(pixels.getWidth());
	const int height = static_cast<int>(pixels.getHeight());
	if (width < minCellSize || height < minCellSize) return false;

	const vector<bool> empty = emptyMask(pixels);

	vector<bool> occupiedColumns(width, false);
	vector<bool> occupiedRows(height, false);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (!empty[y * width + x]) {
				occupiedColumns[x] = true;
				occupiedRows[y] = true;
			}
		}
	}

	columns = bestDivisions(occupiedColumns);
	rows = bestDivisions(occupiedRows);
	if (columns < 1 || rows < 1 || columns * rows < 2) return false;

	frameCount = 0;
	for (int cell = 0; cell < columns * rows; ++cell) {
		const int x0 = (cell % columns) * width / columns;
		const int x1 = (cell % columns + 1) * width / columns;
		const int y0 = (cell / columns) * height / rows;
		const int y1 = (cell / columns + 1) * height / rows;

		bool hasContent = false;
		for (int y = y0; y < y1 && !hasContent; ++y) {
			for (int x = x0; x < x1 && !hasContent; ++x) {
				hasContent = !empty[y * width + x];
			}
		}
		if (hasContent) frameCount = cell + 1;
	}
	return frameCount > 1;
}

size_t SpriteLibrary::add(const SpriteAnimation & animation, shared_ptr<ofImage> sheet) {
	prepare(*sheet);
	sheets[animation.sheetPath] = move(sheet);
	catalog.push_back(animation);
	return catalog.size() - 1;
}

bool SpriteLibrary::import(const ofBuffer & data, const string & sourceName, ImportSettings & settings,
	size_t & index, string & error) {
	auto sheet = make_shared<ofImage>();
	if (data.size() == 0 || !sheet->load(data)) {
		error = "le fichier choisi n'est pas une image valide";
		return false;
	}

	settings.detected = false;
	if (settings.detectGrid) {
		int columns = 0, rows = 0, frames = 0;
		if (detectGrid(sheet->getPixels(), columns, rows, frames)) {
			settings.columns = columns;
			settings.rows = rows;
			settings.frameCount = frames;
			settings.detected = true;
		}
	}

	if (settings.columns < 1 || settings.rows < 1) {
		error = "la grille doit avoir au moins 1 colonne et 1 ligne";
		return false;
	}

	const int cellCount = settings.columns * settings.rows;
	const int frameCount = settings.frameCount > 0 ? min(settings.frameCount, cellCount) : cellCount;
	const string baseName = importedBaseName(sourceName);
	const string relativePath = string(importedFolder) + "/" + baseName + "_"
		+ ofToString(settings.columns) + "x" + ofToString(settings.rows) + "_"
		+ ofToString(frameCount) + "f.png";

	const int existing = findImported(baseName);
	if (existing >= 0 && catalog[existing].sheetPath == relativePath) {
		index = static_cast<size_t>(existing);
		ofLog() << "<feuille de sprites deja importee: " << baseName << ">";
		return true;
	}

	const int frameWidth = static_cast<int>(sheet->getWidth()) / settings.columns;
	const int frameHeight = static_cast<int>(sheet->getHeight()) / settings.rows;
	if (frameWidth < 1 || frameHeight < 1) {
		error = "l'image est trop petite pour une grille de " + ofToString(settings.columns)
			+ " x " + ofToString(settings.rows);
		return false;
	}

	ofDirectory::createDirectory(importedFolder, true, true);
	if (!sheet->save(relativePath)) {
		error = "impossible d'enregistrer " + relativePath;
		return false;
	}

	SpriteAnimation animation;
	animation.name = baseName;
	animation.sheetPath = relativePath;
	animation.frameWidth = frameWidth;
	animation.frameHeight = frameHeight;
	animation.columns = settings.columns;
	animation.frameCount = frameCount;
	animation.fps = settings.fps;

	if (existing >= 0) {
		const string supersededPath = catalog[existing].sheetPath;
		sheets.erase(supersededPath);
		ofFile::removeFile(supersededPath);
		prepare(*sheet);
		sheets[relativePath] = sheet;
		catalog[existing] = animation;
		index = static_cast<size_t>(existing);
	} else {
		index = add(animation, sheet);
	}

	ofLog() << "<feuille de sprites importee: " << ofToDataPath(relativePath, true) << ">";
	return true;
}

shared_ptr<ofImage> SpriteLibrary::sheetFor(const SpriteAnimation & animation) const {
	auto it = sheets.find(animation.sheetPath);
	return it != sheets.end() ? it->second : nullptr;
}
