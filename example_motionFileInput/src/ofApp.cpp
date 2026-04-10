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
    "example_motionFileInput",
    "Drop a video file to start analysis",
    "Keys: V = toggle video, M = toggle motion, arrows = seek once loaded"
  };

  if (!droppedFilePath.empty()) {
    lines.push_back("Loaded file: " + truncatePath(droppedFilePath));
  }

  ofDrawBitmapStringHighlight(ofJoinString(lines, "\n"), 20, 28);
  gui.draw();
}

void ofApp::keyPressed(int key) {
  motionFromVideo.keyPressed(key);
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
  if (dragInfo.files.empty()) {
    return;
  }
  activateFileSource(dragInfo.files.front());
}

void ofApp::activateFileSource(const std::string& path) {
  droppedFilePath = path;
  frameSourcePtr = std::make_shared<VideoFileFrameSource>(path, true);
  motionFromVideo.setFrameSource(frameSourcePtr);
}
