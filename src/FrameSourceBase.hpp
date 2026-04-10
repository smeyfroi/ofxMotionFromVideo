#pragma once

#include "IFrameSource.hpp"
#include "PingPongFbo.h"

class FrameSourceBase : public IFrameSource {
public:
  bool isReady() const override;
  bool isFrameNew() const override { return frameNewThisUpdate; }
  glm::vec2 getSize() const override { return size; }

  const ofFbo& getCurrentFrameFbo() const override;
  const ofFbo& getPreviousFrameFbo() const override;

protected:
  void allocateFrameFbos(glm::vec2 size_);
  void drawTextureToCurrentFrame(const ofTexture& texture, bool mirrorX);
  void resetFrameState();
  void markFrameUpdated();

  PingPongFbo frameFbo;
  glm::vec2 size { 0.0f, 0.0f };
  bool frameNewThisUpdate { false };
  bool hasEverReceivedFrame { false };

private:
  ofFbo emptyFrameFbo;
};
