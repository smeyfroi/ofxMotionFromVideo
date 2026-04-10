#include "ofApp.h"
#include "ofMain.h"

int main() {
  ofGLWindowSettings settings;
  settings.setGLVersion(4, 1);
  settings.setSize(1280, 720);
  auto window = ofCreateWindow(settings);

  ofRunApp(window, std::make_shared<ofApp>());
  ofRunMainLoop();
}
