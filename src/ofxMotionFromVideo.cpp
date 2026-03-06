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

void MotionFromVideo::setPositionSeconds(int seconds) {
  if (isGrabbing) {
    ofLogWarning("MotionFromVideo") << "setPositionSeconds(): not supported for camera source";
    return;
  }
  if (!videoPlayer.isLoaded()) {
    ofLogWarning("MotionFromVideo") << "setPositionSeconds(): video not loaded";
    return;
  }
  float duration = videoPlayer.getDuration();
  if (duration <= 0) {
    ofLogWarning("MotionFromVideo") << "setPositionSeconds(): video has no duration";
    return;
  }
  float position = static_cast<float>(seconds) / duration;
  if (position < 0.0f || position > 1.0f) {
    ofLogWarning("MotionFromVideo") << "setPositionSeconds(): " << seconds << "s is out of range (duration: " << duration << "s)";
    position = std::clamp(position, 0.0f, 1.0f);
  }
  videoPlayer.setPosition(position);
  ofLogNotice("MotionFromVideo") << "setPositionSeconds(): set to " << seconds << "s (position: " << position << ")";
}

void MotionFromVideo::stop() {
  useExternalFrames = false;
  externalCurrentFrameFbo = nullptr;
  externalPreviousFrameFbo = nullptr;
  externalHasNewFrame = false;

  if (videoPlayer.isInitialized()) videoPlayer.close();
  if (videoGrabber.isInitialized()) videoGrabber.close();
}

void MotionFromVideo::setExternalFrames(const ofFbo* currentFrameFbo, const ofFbo* previousFrameFbo, bool hasNewFrame) {
  useExternalFrames = currentFrameFbo && previousFrameFbo;
  externalCurrentFrameFbo = currentFrameFbo;
  externalPreviousFrameFbo = previousFrameFbo;
  externalHasNewFrame = hasNewFrame;

  if (useExternalFrames && externalCurrentFrameFbo && externalCurrentFrameFbo->isAllocated()) {
    glm::vec2 externalSize { externalCurrentFrameFbo->getWidth(), externalCurrentFrameFbo->getHeight() };
    if (!opticalFlowFbo.isAllocated() || externalSize != size) {
      initialiseFbos(externalSize);
      startupFrame = -30;
    }
  }
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
  
  if (!opticalFlowShaderLoaded) {
    opticalFlowShader.load();
    opticalFlowShaderLoaded = true;
  }
}

void MotionFromVideo::update() {
  // External mode: consumer provides current+previous GPU frames.
  if (useExternalFrames) {
    if (!externalCurrentFrameFbo || !externalPreviousFrameFbo) return;
    if (!externalCurrentFrameFbo->isAllocated() || !externalPreviousFrameFbo->isAllocated()) return;

    const glm::vec2 externalSize { externalCurrentFrameFbo->getWidth(), externalCurrentFrameFbo->getHeight() };
    if (!opticalFlowFbo.isAllocated() || externalSize != size) {
      initialiseFbos(externalSize);
      startupFrame = -30;
    }

    if (externalHasNewFrame) {
      if (startupFrame == 0) {
        opticalFlowFbo.begin();
        // OpticalFlowShader takes a non-const lastFrame argument, but does not modify it.
        auto& lastFrame = const_cast<ofFbo&>(*externalPreviousFrameFbo);
        opticalFlowShader.render(size.x, size.y, *externalCurrentFrameFbo, lastFrame);
        opticalFlowFbo.end();
      } else {
        startupFrame++;
      }
    }

    // CPU sampling only when we have new motion data.
    if (cpuSamplingEnabled && externalHasNewFrame && startupFrame == 0) {
      opticalFlowFbo.readToPixels(opticalFlowPixels);
    }

    return;
  }

  // Internal mode: own and advance camera/file source.
  if (!videoFbo.getSource().isAllocated()) return;

  // Guard against update after stop() - video may have been closed
  if (isGrabbing) {
    if (!videoGrabber.isInitialized()) return;
  } else {
    if (!videoPlayer.isLoaded()) return;
  }

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

    if (cpuSamplingEnabled && startupFrame == 0) {
      opticalFlowFbo.readToPixels(opticalFlowPixels);
      //  if (isGrabbing) opticalFlowPixels.mirror(false, true); // ************************
    }
  }
}

// Returns normalised position and scaled velocity
std::optional<glm::vec4> MotionFromVideo::trySampleMotion() {
  if (!isReady()) return {};
  
  float x = ofRandom(size.x-1);
  float y = ofRandom(size.y-1);
  if (!cpuSamplingEnabled) return {};

  auto c = opticalFlowPixels.getColor(x, y);
  float speed = std::sqrt(c.r * c.r + c.g * c.g);
  if (speed >= minSpeedMagnitude) {
    return { glm::vec4 { x / size.x, y / size.y, c.r / size.x, c.g / size.x } };
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
    parameters.add(minSpeedMagnitude);
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
  if (useExternalFrames && externalCurrentFrameFbo && externalCurrentFrameFbo->isAllocated()) {
    drawFbo(*externalCurrentFrameFbo, /*mirrored*/ false);
    return;
  }
  drawFbo(videoFbo.getSource(), isGrabbing);
}

// For debug only
void MotionFromVideo::drawMotion() {
  drawFbo(opticalFlowFbo, isGrabbing);
}
