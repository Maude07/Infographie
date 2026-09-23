#include "imageExporter.h"

void ImageExporter::start() {
	folder = "export_" + ofGetTimestampString("%Y%m%d_%H%M%S");
	ofDirectory::createDirectory(folder, true, true);
	frame_count = 0;
	start_time = ofGetElapsedTimef();
	last_capture_time = -1.0f;
	is_recording = true;
	ofLog() << "<export démarré: " << folder << ">";
}

void ImageExporter::stop() {
	is_recording = false;
	ofLog() << "<export terminé: " << frame_count << " images>";
}

void ImageExporter::toggle() {
	is_recording ? stop() : start();
}

void ImageExporter::capture() {
	if (!is_recording) return;

	float now = ofGetElapsedTimef();

	// arrêt automatique une fois la durée écoulée
	if (duration > 0 && (now - start_time) >= duration) {
		stop();
		return;
	}

	// respecter la cadence demandée
	if (last_capture_time >= 0 && (now - last_capture_time) < 1.0f / fps) return;
	last_capture_time = now;

	image.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	std::string name = folder + "/frame_" + ofToString(frame_count, 5, '0') + ".png";
	image.save(name);
	frame_count++;
}
