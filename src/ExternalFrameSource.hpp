#pragma once

#include "IFrameSource.hpp"

class ExternalFrameSource : public IFrameSource {
public:
  void setFrames(const ofFbo* currentFrameFbo,
                 const ofFbo* previousFrameFbo,
                 bool frameNew,
                 bool mirrored = false);

  void update() override {}
  void stop() override;

  bool isReady() const override;
  bool isFrameNew() const override { return frameNewThisUpdate; }
  bool isMirrored() const override { return mirrored; }
  glm::vec2 getSize() const override;

  const ofFbo& getCurrentFrameFbo() const override;
  const ofFbo& getPreviousFrameFbo() const override;

private:
  const ofFbo* currentFrameFbo { nullptr };
  const ofFbo* previousFrameFbo { nullptr };
  bool frameNewThisUpdate { false };
  bool mirrored { false };
  ofFbo emptyFrameFbo;
};
