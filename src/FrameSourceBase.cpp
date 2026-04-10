#include "FrameSourceBase.hpp"

#include "ofGraphics.h"

bool FrameSourceBase::isReady() const {
  return frameFbo.getSource().isAllocated() && hasEverReceivedFrame;
}

const ofFbo& FrameSourceBase::getCurrentFrameFbo() const {
  return frameFbo.getSource().isAllocated() ? frameFbo.getSource() : emptyFrameFbo;
}

const ofFbo& FrameSourceBase::getPreviousFrameFbo() const {
  return frameFbo.getTarget().isAllocated() ? frameFbo.getTarget() : emptyFrameFbo;
}

void FrameSourceBase::allocateFrameFbos(glm::vec2 size_) {
  size = size_;

  ofFboSettings settings;
  settings.width = size.x;
  settings.height = size.y;
  settings.internalformat = GL_RGB8;
  settings.useDepth = false;
  settings.useStencil = false;
  settings.depthStencilAsTexture = false;
  settings.numSamples = 0;
  settings.minFilter = GL_NEAREST;
  settings.maxFilter = GL_NEAREST;
  settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
  settings.wrapModeVertical = GL_CLAMP_TO_EDGE;
  settings.textureTarget = GL_TEXTURE_2D;

  frameFbo.allocate(settings);
  frameFbo.clear(0, 255);
  resetFrameState();
}

void FrameSourceBase::drawTextureToCurrentFrame(const ofTexture& texture, bool mirrorX) {
  frameFbo.getSource().begin();
  ofEnableBlendMode(OF_BLENDMODE_DISABLED);

  if (mirrorX) {
    texture.draw(texture.getWidth(), 0.0f, -texture.getWidth(), texture.getHeight());
  } else {
    texture.draw(0.0f, 0.0f);
  }

  frameFbo.getSource().end();
}

void FrameSourceBase::resetFrameState() {
  frameNewThisUpdate = false;
  hasEverReceivedFrame = false;
}

void FrameSourceBase::markFrameUpdated() {
  frameNewThisUpdate = true;
  hasEverReceivedFrame = true;
}
