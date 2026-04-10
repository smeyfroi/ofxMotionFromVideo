#pragma once

#include "FrameSourceBase.hpp"

class CameraFrameSource : public FrameSourceBase {
public:
  CameraFrameSource() = default;
  CameraFrameSource(int deviceId, glm::vec2 desiredSize);

  bool setup(int deviceId, glm::vec2 desiredSize);
  void update() override;
  void stop() override;
  bool isMirrored() const override { return false; }

private:
  ofVideoGrabber videoGrabber;
};
