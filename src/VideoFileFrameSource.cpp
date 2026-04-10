#include "VideoFileFrameSource.hpp"

#include <algorithm>

#include "ofAppRunner.h"
#include "ofLog.h"

VideoFileFrameSource::VideoFileFrameSource(const std::string& path, bool mute) {
  load(path, mute);
}

bool VideoFileFrameSource::load(const std::string& path, bool mute) {
  stop();

  if (!videoPlayer.load(path)) {
    ofLogError("VideoFileFrameSource") << "Failed to load video from " << path;
    return false;
  }

  if (mute) {
    videoPlayer.setVolume(0.0f);
  }
  videoPlayer.play();

  allocateFrameFbos(videoPlayer.getSize());
  return true;
}

void VideoFileFrameSource::update() {
  frameNewThisUpdate = false;

  if (!videoPlayer.isLoaded()) {
    return;
  }

  videoPlayer.update();
  if (!videoPlayer.isFrameNew()) {
    return;
  }

  frameFbo.swap();
  drawTextureToCurrentFrame(videoPlayer.getTexture(), /*mirrorX*/ false);
  markFrameUpdated();
}

void VideoFileFrameSource::stop() {
  if (videoPlayer.isInitialized()) {
    videoPlayer.close();
  }
  size = { 0.0f, 0.0f };
  resetFrameState();
}

bool VideoFileFrameSource::keyPressed(int key) {
  if (!videoPlayer.isLoaded()) {
    return false;
  }

  int skipSeconds = ofGetKeyPressed(OF_KEY_SHIFT) ? 60 : 10;
  if (key == OF_KEY_UP) {
    setPositionSeconds(static_cast<int>(videoPlayer.getPosition() * videoPlayer.getDuration()) + skipSeconds);
    return true;
  }
  if (key == OF_KEY_DOWN) {
    setPositionSeconds(static_cast<int>(videoPlayer.getPosition() * videoPlayer.getDuration()) - skipSeconds);
    return true;
  }

  return false;
}

void VideoFileFrameSource::setPositionSeconds(int seconds) {
  if (!videoPlayer.isLoaded()) {
    return;
  }

  float duration = videoPlayer.getDuration();
  if (duration <= 0.0f) {
    ofLogWarning("VideoFileFrameSource") << "setPositionSeconds(): video has no duration";
    return;
  }

  float position = static_cast<float>(seconds) / duration;
  position = std::clamp(position, 0.0f, 1.0f);
  videoPlayer.setPosition(position);
}
