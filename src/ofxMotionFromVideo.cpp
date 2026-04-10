#include "ofxMotionFromVideo.h"

#include <cmath>

namespace {

void drawFbo(const ofFbo& fbo, bool mirrored) {
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofSetColor(ofFloatColor { 1.0f, 1.0f, 1.0f, 0.25f });
  ofPushMatrix();
  if (mirrored) {
    ofTranslate(1.0f, 0.0f);
    ofScale(-1.0f, 1.0f);
  }
  fbo.draw(0.0f, 0.0f, 1.0f, 1.0f);
  ofPopMatrix();
  ofPopStyle();
}

} // namespace

MotionFromVideo::~MotionFromVideo() {
  stop();
}

void MotionFromVideo::setFrameSource(std::shared_ptr<IFrameSource> frameSource) {
  frameSourcePtr = std::move(frameSource);
  startupFrame = -30;
}

void MotionFromVideo::initialiseCamera(int deviceId, glm::vec2 size_) {
  auto source = std::make_shared<CameraFrameSource>();
  if (source->setup(deviceId, size_)) {
    setFrameSource(source);
  }
}

void MotionFromVideo::load(const std::string& path, bool mute) {
  auto source = std::make_shared<VideoFileFrameSource>();
  if (source->load(path, mute)) {
    setFrameSource(source);
  }
}

void MotionFromVideo::setPositionSeconds(int seconds) {
  if (frameSourcePtr) {
    frameSourcePtr->setPositionSeconds(seconds);
  }
}

void MotionFromVideo::stop() {
  if (frameSourcePtr) {
    frameSourcePtr->stop();
  }
  frameSourcePtr.reset();
  externalFrameSourcePtr.reset();
  startupFrame = -30;
  size = { 0.0f, 0.0f };
}

void MotionFromVideo::setExternalFrames(const ofFbo* currentFrameFbo,
                                        const ofFbo* previousFrameFbo,
                                        bool hasNewFrame,
                                        bool mirrored) {
  if (!externalFrameSourcePtr) {
    externalFrameSourcePtr = std::make_shared<ExternalFrameSource>();
  }
  externalFrameSourcePtr->setFrames(currentFrameFbo, previousFrameFbo, hasNewFrame, mirrored);
  if (frameSourcePtr != externalFrameSourcePtr) {
    setFrameSource(externalFrameSourcePtr);
  }
}

void MotionFromVideo::initialiseFbos(glm::vec2 size_) {
  size = size_;

  ofFboSettings settings;
  settings.width = size.x;
  settings.height = size.y;
  settings.internalformat = GL_RGB16F;
  settings.useDepth = false;
  settings.useStencil = false;
  settings.depthStencilAsTexture = false;
  settings.numSamples = 0;
  settings.minFilter = GL_LINEAR;
  settings.maxFilter = GL_LINEAR;
  settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
  settings.wrapModeVertical = GL_CLAMP_TO_EDGE;
  settings.textureTarget = GL_TEXTURE_2D;
  opticalFlowFbo.allocate(settings);

  opticalFlowFbo.begin();
  ofClear(ofFloatColor { 0.0f, 0.0f, 0.0f, 1.0f });
  opticalFlowFbo.end();

  if (!opticalFlowShaderLoaded) {
    opticalFlowShader.load();
    opticalFlowShaderLoaded = true;
  }
}

void MotionFromVideo::update() {
  if (!frameSourcePtr) {
    return;
  }

  frameSourcePtr->update();
  if (!frameSourcePtr->isReady()) {
    return;
  }

  const glm::vec2 sourceSize = frameSourcePtr->getSize();
  if (!opticalFlowFbo.isAllocated() || sourceSize != size) {
    initialiseFbos(sourceSize);
    startupFrame = -30;
  }

  if (!frameSourcePtr->isFrameNew()) {
    return;
  }

  if (startupFrame == 0) {
    opticalFlowFbo.begin();
    auto& previousFrame = const_cast<ofFbo&>(frameSourcePtr->getPreviousFrameFbo());
    opticalFlowShader.render(size.x, size.y, frameSourcePtr->getCurrentFrameFbo(), previousFrame);
    opticalFlowFbo.end();

    if (cpuSamplingEnabled) {
      opticalFlowFbo.readToPixels(opticalFlowPixels);
    }
  } else {
    startupFrame++;
  }
}

std::optional<glm::vec4> MotionFromVideo::trySampleMotion() {
  if (!isReady() || !cpuSamplingEnabled) {
    return {};
  }

  float x = ofRandom(size.x - 1.0f);
  float y = ofRandom(size.y - 1.0f);
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

bool MotionFromVideo::isReady() const {
  return frameSourcePtr && frameSourcePtr->isReady() && startupFrame == 0;
}

bool MotionFromVideo::keyPressed(int key) {
  if (frameSourcePtr && frameSourcePtr->keyPressed(key)) {
    return true;
  }
  if (key == 'V') {
    videoVisible = !videoVisible;
    return true;
  }
  if (key == 'M') {
    motionVisible = !motionVisible;
    return true;
  }
  return false;
}

void MotionFromVideo::draw() {
  if (videoVisible) {
    drawVideo();
  }
  if (motionVisible) {
    drawMotion();
  }
}

const ofFbo& MotionFromVideo::getVideoFbo() const {
  return frameSourcePtr ? frameSourcePtr->getCurrentFrameFbo() : emptyFrameFbo;
}

void MotionFromVideo::drawVideo() {
  const ofFbo& videoFbo = getVideoFbo();
  if (!videoFbo.isAllocated()) {
    return;
  }
  drawFbo(videoFbo, frameSourcePtr && frameSourcePtr->isMirrored());
}

void MotionFromVideo::drawMotion() {
  if (!opticalFlowFbo.isAllocated()) {
    return;
  }
  drawFbo(opticalFlowFbo, frameSourcePtr && frameSourcePtr->isMirrored());
}
