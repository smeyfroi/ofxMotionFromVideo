#include "ofApp.h"

namespace {

constexpr float FLUID_SIM_SCALE = 0.5f;

std::string truncatePath(const std::string& path, std::size_t maxLength = 72) {
  if (path.size() <= maxLength) {
    return path;
  }
  return "..." + path.substr(path.size() - (maxLength - 3));
}

} // namespace

void ofApp::setup() {
  ofDisableArbTex();
  ofEnableAlphaBlending();

  externalFrameSourcePtr = std::make_shared<ExternalFrameSource>();
  motionFromVideo.setFrameSource(externalFrameSourcePtr);
  motionFromVideo.setVideoVisible(true);
  motionFromVideo.setMotionVisible(true);

  activateCameraSource();

  fluidSimulation.setup(ofGetWindowSize() * FLUID_SIM_SCALE);
  parameters.add(motionFromVideo.getParameterGroup());
  parameters.add(fluidSimulation.getParameterGroup());
  gui.setup(parameters);

  addTextureShader.load();
}

void ofApp::update() {
  if (upstreamSourcePtr) {
    upstreamSourcePtr->update();
    externalFrameSourcePtr->setFrames(&upstreamSourcePtr->getCurrentFrameFbo(),
                                      &upstreamSourcePtr->getPreviousFrameFbo(),
                                      upstreamSourcePtr->isFrameNew(),
                                      upstreamSourcePtr->isMirrored());
  }

  motionFromVideo.update();

  if (motionFromVideo.getVideoFbo().isAllocated()) {
    addTextureShader.render(fluidSimulation.getFlowValuesFbo(), motionFromVideo.getVideoFbo().getTexture(), 0.001f);
  }
  if (motionFromVideo.getMotionFbo().isAllocated()) {
    addTextureShader.render(fluidSimulation.getFlowVelocitiesFbo(), motionFromVideo.getMotionFbo().getTexture(), 0.01f);
  }

  fluidSimulation.update();
}

void ofApp::draw() {
  ofSetWindowTitle(ofToString(ofGetFrameRate()));
  fluidSimulation.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

  ofPushMatrix();
  ofScale(ofGetWindowWidth(), ofGetWindowHeight());
  motionFromVideo.draw();
  ofPopMatrix();

  std::vector<std::string> lines {
    "example_VideoMotionIntoFluid",
    "External frame bridge demo",
    "Mode: " + sourceLabel,
    "Keys: c = camera, V = toggle video, M = toggle motion",
    "Drop a video file to switch the upstream source (optional)"
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

  if (upstreamSourcePtr && upstreamSourcePtr->keyPressed(key)) {
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
  setUpstreamSource(std::make_shared<CameraFrameSource>(0, glm::vec2 { 1280.0f, 720.0f }), "camera");
}

void ofApp::activateFileSource(const std::string& path) {
  droppedFilePath = path;
  setUpstreamSource(std::make_shared<VideoFileFrameSource>(path, true), "file");
}

void ofApp::setUpstreamSource(std::shared_ptr<IFrameSource> source, const std::string& label) {
  upstreamSourcePtr = std::move(source);
  sourceLabel = label;
}
