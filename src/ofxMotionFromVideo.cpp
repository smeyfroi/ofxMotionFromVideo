#include "ofxMotionFromVideo.h"

MotionFromVideo::MotionFromVideo() {
}

MotionFromVideo::~MotionFromVideo() {
  video.close();
}

void MotionFromVideo::load(const std::string& path, bool mute) {
  video.load(path);
  if (mute) video.setVolume(0);
  video.play();
  videoFbo.allocate(video.getWidth(), video.getHeight(), GL_RGB);
  videoFbo.getSource().begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0});
  videoFbo.getSource().end();
  videoFbo.getTarget().begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0});
  videoFbo.getTarget().end();

  opticalFlowFbo.allocate(video.getWidth(), video.getHeight(), GL_RGB32F);
  opticalFlowFbo.begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0 });
  opticalFlowFbo.end();
  
  opticalFlowShader.load();
}

void MotionFromVideo::update() {
  video.update();

  if (video.isFrameNew()) {
    // target is last frame, source is current frame
    videoFbo.swap();
    videoFbo.getSource().begin();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    video.getTexture().draw(0, 0);
    videoFbo.getSource().end();
    
    if (doneFirstMotionRender) { // initial skip to avoid huge diff in first frame
      opticalFlowFbo.begin();
      opticalFlowShader.render(opticalFlowFbo.getWidth(), opticalFlowFbo.getHeight(), videoFbo.getSource(), videoFbo.getTarget());
      opticalFlowFbo.end();
    }
    
    doneFirstMotionRender = true;
  }

  opticalFlowFbo.readToPixels(opticalFlowPixels);
}

std::optional<glm::vec4> MotionFromVideo::trySampleMotion() const {
  float x = ofRandom(opticalFlowPixels.getWidth());
  float y = ofRandom(opticalFlowPixels.getHeight());
  auto c = opticalFlowPixels.getColor(x, y);
  if (c.r > xFlowThresholdPos || c.r < xFlowThresholdNeg || c.g > yFlowThresholdPos || c.g < yFlowThresholdNeg) {
    return { glm::vec4 {
      x,
      y,
      c.r,
      c.g
    } };
  }
  return std::nullopt;
}

const std::string MotionFromVideo::getParameterGroupName() {
  return "MotionFromVideo";
}

ofParameterGroup& MotionFromVideo::getParameterGroup() {
  if (parameters.size() == 0) {
    parameters.add(opticalFlowShader.getParameterGroup());
    parameters.add(xFlowThresholdNeg);
    parameters.add(xFlowThresholdPos);
    parameters.add(yFlowThresholdNeg);
    parameters.add(yFlowThresholdPos);
  }
  return parameters;
}

bool MotionFromVideo::keyPressed(int key) {
  // TODO: toggle video and motion layers. Needs a draw() too
  return false;
}

void MotionFromVideo::drawVideo() {
  videoFbo.draw(0.0, 0.0, ofGetWindowWidth(), ofGetWindowHeight());
}

void MotionFromVideo::drawMotion() {
  opticalFlowFbo.draw(0.0, 0.0, ofGetWindowWidth(), ofGetWindowHeight());
}
