#include "ofxMotionFromVideo.h"

MotionFromVideo::~MotionFromVideo() {
  if (videoPlayer.isInitialized()) videoPlayer.close();
  if (videoGrabber.isInitialized()) videoGrabber.close();
}

void MotionFromVideo::initialiseCamera(int deviceID, glm::vec2 size) {
  isGrabbing = true;
  const auto& devices = videoGrabber.listDevices(); // dumps device IDs to stdout
  videoGrabber.setDeviceID(deviceID);
//  videoGrabber.setVerbose(true);
  videoGrabber.setPixelFormat(OF_PIXELS_RGB);
  videoGrabber.setDesiredFrameRate(30);
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
  
  {
    ofFboSettings s;
    s.width = size.x;
    s.height = size.y;
    s.internalformat = GL_RGB8;
    s.useDepth = false;
    s.useStencil = false;
    s.depthStencilAsTexture = false;
    s.numSamples = 0;
    s.minFilter = GL_NEAREST;
    s.maxFilter = GL_NEAREST;
    s.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    s.wrapModeVertical = GL_CLAMP_TO_EDGE;
    s.textureTarget = GL_TEXTURE_2D;
    videoFbo.allocate(s);
  }
  videoFbo.clear(0, 255);

  {
    ofFboSettings s;
    s.width = size.x;
    s.height = size.y;
    s.internalformat = GL_RG16F; // 3 channels half-float because we keep an ofPixels from this, so can't use just two channels TODO revisit this when we aren't copying to ofPixels
    s.useDepth = false;
    s.useStencil = false;
    s.depthStencilAsTexture = false;
    s.numSamples = 0;
    s.minFilter = GL_LINEAR;  // interpolate the field
    s.maxFilter = GL_LINEAR;
    s.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    s.wrapModeVertical = GL_CLAMP_TO_EDGE;
    s.textureTarget = GL_TEXTURE_2D;
    opticalFlowFbo.allocate(s);
  }
  opticalFlowFbo.begin();
  ofClear(ofFloatColor { 0.0, 0.0, 0.0, 1.0 });
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
}

const std::string MotionFromVideo::getParameterGroupName() const {
  return "MotionFromVideo";
}

ofParameterGroup& MotionFromVideo::getParameterGroup() {
  if (parameters.size() == 0) {
    parameters.setName(getParameterGroupName());
    parameters.add(opticalFlowShader.getParameterGroup());
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
  // TODO: mute/unmute if playing a video file
  return false;
}

// For debug only
void drawFbo(const ofFbo& fbo, bool mirrored) {
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofSetColor(ofFloatColor { 1.0, 1.0, 1.0, 0.25 });
  ofPushMatrix();
  if (mirrored) {
    ofTranslate(1.0, 0.0);
    ofScale(-1, 1);
  }
  fbo.draw(0.0, 0.0, 1.0, 1.0);
  ofPopMatrix();
  ofPopStyle();
}

// For debug only
void MotionFromVideo::draw() {
  if (videoVisible) drawVideo();
  if (motionVisible) drawMotion();
}

// For debug only
void MotionFromVideo::drawVideo() {
  drawFbo(videoFbo.getSource(), isGrabbing);
}

// For debug only
void MotionFromVideo::drawMotion() {
  drawFbo(opticalFlowFbo, isGrabbing);
}
