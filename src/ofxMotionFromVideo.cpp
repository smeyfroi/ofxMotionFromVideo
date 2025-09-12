#include "ofxMotionFromVideo.h"

MotionFromVideo::MotionFromVideo() :
videoVisible { false },
motionVisible { false }
{}

MotionFromVideo::~MotionFromVideo() {
  if (videoPlayer.isInitialized()) videoPlayer.close();
//  if (videoGrabber.isInitialized()) videoGrabber.close();
}

void MotionFromVideo::initialiseCamera(int deviceID, glm::vec2 size) {
  isGrabbing = true;
  const auto& devices = videoGrabber.listDevices(); // dumps device IDs to stdout
  videoGrabber.setDeviceID(deviceID);
  videoGrabber.setup(size.x, size.y);
//  std::for_each(devices.cbegin(), devices.cend(), [](const auto& d) {
//    ofLogNotice() << d.id << " : " << d.deviceName;
//    const auto& formats = d.formats;
//    std::for_each(formats.cbegin(), formats.cend(), [](const auto& f) {
//      ofLogNotice() << "  " << f.pixelFormat << " , " << f.width << " x " << f.height << " , "; // << framerates;
//    });
//  });
  initialiseFbos(videoGrabber.getSize());
}

void MotionFromVideo::load(const std::string& path, bool mute) {
  isGrabbing = false;
  videoPlayer.load(path);
  if (mute) videoPlayer.setVolume(0);
  videoPlayer.play();
  initialiseFbos(videoPlayer.getSize());
}

void MotionFromVideo::initialiseFbos(glm::vec2 size_) {
  size = size_;
  videoFbo.allocate(size.x, size.y, GL_RGB);
  videoFbo.getSource().begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0});
  videoFbo.getSource().end();
  videoFbo.getTarget().begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0});
  videoFbo.getTarget().end();

  opticalFlowFbo.allocate(size.x, size.y, GL_RGB32F);
  opticalFlowFbo.begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0 });
  opticalFlowFbo.end();
  
  opticalFlowShader.load();
}

void MotionFromVideo::update() {
  if (!videoFbo.getSource().isAllocated()) return;
  
  bool hasNewFrame = false;
  if (isGrabbing) {
    videoGrabber.update();
    hasNewFrame = videoGrabber.isFrameNew();
  } else {
    videoPlayer.update();
    hasNewFrame = videoPlayer.isFrameNew();
  }
  
  if (hasNewFrame) {
    // target is last frame, source is current frame
    videoFbo.swap();
    videoFbo.getSource().begin();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    auto& texture = isGrabbing ? videoGrabber.getTexture() : videoPlayer.getTexture();
    texture.draw(0, 0);
    videoFbo.getSource().end();
      
    if (startupFrame == 0) {
      opticalFlowFbo.begin();
      opticalFlowShader.render(size.x, size.y, videoFbo.getSource(), videoFbo.getTarget());
      opticalFlowFbo.end();
    } else {
      startupFrame++;
    }
  }

  opticalFlowFbo.readToPixels(opticalFlowPixels);
  if (isGrabbing) opticalFlowPixels.mirror(false, true); // ************************
}

std::optional<glm::vec4> MotionFromVideo::trySampleMotion() {
  if (!isReady()) return {};
  
  float x = ofRandom(size.x);
  float y = ofRandom(size.y);
  auto c = opticalFlowPixels.getColor(x, y);
  if (c.r > xFlowThresholdPos || c.r < xFlowThresholdNeg || c.g > yFlowThresholdPos || c.g < yFlowThresholdNeg) {
    return { glm::vec4 { x, y, c.r, c.g } };
  }
  return {};
}

const std::string MotionFromVideo::getParameterGroupName() const {
  return "MotionFromVideo";
}

ofParameterGroup& MotionFromVideo::getParameterGroup() {
  if (parameters.size() == 0) {
    parameters.setName(getParameterGroupName());
    parameters.add(opticalFlowShader.getParameterGroup());
    parameters.add(xFlowThresholdNeg);
    parameters.add(xFlowThresholdPos);
    parameters.add(yFlowThresholdNeg);
    parameters.add(yFlowThresholdPos);
  }
  return parameters;
}

bool MotionFromVideo::keyPressed(int key) {
  if (key == 'V') {
    videoVisible = !videoVisible;
    return true;
  } else if (key == 'M') {
    motionVisible = !motionVisible;
    return true;
  }
  // TODO: mute/unmute
  return false;
}

void MotionFromVideo::draw() {
  if (videoVisible) drawVideo();
  if (motionVisible) drawMotion();
}

void drawFbo(const ofFbo& fbo, bool mirrored) {
  ofPushMatrix();
  if (mirrored) {
    ofTranslate(1.0, 0.0);
    ofScale(-1, 1);
  }
  fbo.draw(0.0, 0.0, 1.0, 1.0);
  ofPopMatrix();
}

void MotionFromVideo::drawVideo() {
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofSetColor(ofFloatColor { 1.0, 1.0, 1.0, 0.25 });
  drawFbo(videoFbo.getSource(), isGrabbing);
  ofPopStyle();
}

void MotionFromVideo::drawMotion() {
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofSetColor(ofFloatColor { 1.0, 1.0, 1.0, 0.25 });
  drawFbo(opticalFlowFbo, isGrabbing);
  ofPopStyle();
}
