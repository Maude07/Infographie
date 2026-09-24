#include "imageExporter.h"

using namespace std;

void ImageExporter::start() {
	folder = "export_" + ofGetTimestampString("%Y%m%d_%H%M%S");
	ofDirectory::createDirectory(folder, true, true);
	frameCount = 0;
	startTime = ofGetElapsedTimef();
	lastCaptureTime = -1.0f;
	isRecording = true;
	ofLog() << "<export démarré: " << folder << ">";
}

void ImageExporter::stop() {
	isRecording = false;
	ofLog() << "<export terminé: " << frameCount << " images>";
}

void ImageExporter::toggle() {
	isRecording ? stop() : start();
}

void ImageExporter::capture() {
	if (!isRecording) return;

	float now = ofGetElapsedTimef();

	if (duration > 0 && (now - startTime) >= duration) {
		stop();
		return;
	}

	if (lastCaptureTime >= 0 && (now - lastCaptureTime) < 1.0f / fps) return;
	lastCaptureTime = now;

	image.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	string name = folder + "/frame_" + ofToString(frameCount, 5, '0') + ".png";
	image.save(name);
	frameCount++;
}
