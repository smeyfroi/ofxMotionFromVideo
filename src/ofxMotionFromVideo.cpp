#include "ofxMotionFromVideo.h"

MotionFromVideo::~MotionFromVideo() {
  stop();
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
  if (!videoPlayer.load(path)) {
    ofLogError("MotionFromVideo") << "load(): failed to load video from " << path;
    return;
  }
  if (mute) videoPlayer.setVolume(0);
  videoPlayer.play();
  initialiseFbos(videoPlayer.getSize());
}

void MotionFromVideo::stop() {
  if (videoPlayer.isInitialized()) videoPlayer.close();
  if (videoGrabber.isInitialized()) videoGrabber.close();
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
    s.internalformat = GL_RGB16F; // 3 channels half-float because we keep an ofPixels from this, so can't use just two channels
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
    if (isGrabbing) {
      auto& texture = videoGrabber.getTexture();
      texture.draw(texture.getWidth(), 0, -texture.getWidth(), texture.getHeight());
    } else {
      auto& texture = videoPlayer.getTexture();
      texture.draw(0.0, 0.0);
    }
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
//  if (isGrabbing) opticalFlowPixels.mirror(false, true); // ************************
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
  // TODO: mute/unmute if playing a video file
  return false;
}

// For debug only
// Assumes normalised viewport coords
void drawFbo(const ofFbo& fbo, bool mirrored) {
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofSetColor(ofFloatColor { 1.0, 1.0, 1.0, 0.25 });
  ofPushMatrix();
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
