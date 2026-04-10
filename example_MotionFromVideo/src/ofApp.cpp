#include "ofApp.h"

namespace {

std::string truncatePath(const std::string& path, std::size_t maxLength = 72) {
  if (path.size() <= maxLength) {
    return path;
  }
  return "..." + path.substr(path.size() - (maxLength - 3));
}

} // namespace

void ofApp::setup() {
  ofDisableArbTex();
  ofSetFrameRate(30);

  activateCameraSource();

  motionFromVideo.setVideoVisible(true);
  motionFromVideo.setMotionVisible(true);

  parameters.add(motionFromVideo.getParameterGroup());
  gui.setup(parameters);
}

void ofApp::update() {
  motionFromVideo.update();
}

void ofApp::draw() {
  ofBackground(0);

  ofPushMatrix();
  ofScale(ofGetWidth(), ofGetHeight());
  motionFromVideo.draw();
  ofPopMatrix();

  std::vector<std::string> lines {
    "example_MotionFromVideo",
    "Mode: " + sourceLabel,
    "Keys: c = camera, V = toggle video, M = toggle motion",
    "Drop a video file to switch to file playback (optional)",
    "Arrow keys seek when using a file source"
  };

  if (!droppedFilePath.empty()) {
    lines.push_back("Last dropped file: " + truncatePath(droppedFilePath));
  }

  ofDrawBitmapStringHighlight(ofJoinString(lines, "\n"), 20, 28);
  gui.draw();
}

void ofApp::keyPressed(int key) {
  if (key == 'c' || key == 'C') {
    activateCameraSource();
    return;
  }

  motionFromVideo.keyPressed(key);
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
  if (dragInfo.files.empty()) {
    return;
  }

  activateFileSource(dragInfo.files.front());
}

void ofApp::activateCameraSource() {
  setSource(std::make_shared<CameraFrameSource>(0, glm::vec2 { 1280.0f, 720.0f }), "camera");
}

void ofApp::activateFileSource(const std::string& path) {
  droppedFilePath = path;
  setSource(std::make_shared<VideoFileFrameSource>(path, true), "file");
}

void ofApp::setSource(std::shared_ptr<IFrameSource> source, const std::string& label) {
  frameSourcePtr = std::move(source);
  sourceLabel = label;
  motionFromVideo.setFrameSource(frameSourcePtr);
}
