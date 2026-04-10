#include "CameraFrameSource.hpp"

#include "ofLog.h"
#include "ofPixels.h"

CameraFrameSource::CameraFrameSource(int deviceId, glm::vec2 desiredSize) {
  setup(deviceId, desiredSize);
}

bool CameraFrameSource::setup(int deviceId, glm::vec2 desiredSize) {
  stop();

  videoGrabber.listDevices();
  videoGrabber.setDeviceID(deviceId);
  videoGrabber.setPixelFormat(OF_PIXELS_RGB);
  videoGrabber.setDesiredFrameRate(30);

  bool ok = videoGrabber.setup(static_cast<int>(desiredSize.x), static_cast<int>(desiredSize.y));
  if (!ok) {
    ofLogError("CameraFrameSource") << "Failed to setup camera deviceId=" << deviceId;
    return false;
  }

  allocateFrameFbos(videoGrabber.getSize());
  return true;
}

void CameraFrameSource::update() {
  frameNewThisUpdate = false;

  if (!videoGrabber.isInitialized()) {
    return;
  }

  videoGrabber.update();
  if (!videoGrabber.isFrameNew()) {
    return;
  }

  frameFbo.swap();
  drawTextureToCurrentFrame(videoGrabber.getTexture(), /*mirrorX*/ true);
  markFrameUpdated();
}

void CameraFrameSource::stop() {
  if (videoGrabber.isInitialized()) {
    videoGrabber.close();
  }
  size = { 0.0f, 0.0f };
  resetFrameState();
}
