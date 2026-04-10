#pragma once

#include "FrameSourceBase.hpp"

#include <string>

class VideoFileFrameSource : public FrameSourceBase {
public:
  VideoFileFrameSource() = default;
  VideoFileFrameSource(const std::string& path, bool mute = true);

  bool load(const std::string& path, bool mute = true);
  void update() override;
  void stop() override;
  bool keyPressed(int key) override;
  void setPositionSeconds(int seconds) override;

private:
  ofVideoPlayer videoPlayer;
};
