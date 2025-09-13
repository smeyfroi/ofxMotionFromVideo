#include "ofApp.h"
#include "ofAppGLFWWindow.h"

int main(){
  ofGLWindowSettings settings;
  settings.setGLVersion(4,1);
  settings.setSize(1920, 1080);
  auto window = ofCreateWindow(settings);

  ofRunApp(window, std::make_shared<ofApp>());
  ofRunMainLoop();
}
