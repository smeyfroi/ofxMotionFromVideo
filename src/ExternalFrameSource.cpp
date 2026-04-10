#include "ExternalFrameSource.hpp"

void ExternalFrameSource::setFrames(const ofFbo* currentFrameFbo_,
                                    const ofFbo* previousFrameFbo_,
                                    bool frameNew,
                                    bool mirrored_) {
  currentFrameFbo = currentFrameFbo_;
  previousFrameFbo = previousFrameFbo_;
  frameNewThisUpdate = frameNew;
  mirrored = mirrored_;
}

void ExternalFrameSource::stop() {
  currentFrameFbo = nullptr;
  previousFrameFbo = nullptr;
  frameNewThisUpdate = false;
  mirrored = false;
}

bool ExternalFrameSource::isReady() const {
  return currentFrameFbo && previousFrameFbo && currentFrameFbo->isAllocated() && previousFrameFbo->isAllocated();
}

glm::vec2 ExternalFrameSource::getSize() const {
  if (!isReady()) {
    return { 0.0f, 0.0f };
  }
  return { currentFrameFbo->getWidth(), currentFrameFbo->getHeight() };
}

const ofFbo& ExternalFrameSource::getCurrentFrameFbo() const {
  return isReady() ? *currentFrameFbo : emptyFrameFbo;
}

const ofFbo& ExternalFrameSource::getPreviousFrameFbo() const {
  return isReady() ? *previousFrameFbo : emptyFrameFbo;
}
