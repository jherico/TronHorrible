#pragma once

#include "App.hpp"
#include "Stereo.hpp"

namespace Encom13 {

enum Eye {
    LEFT = 0,
    RIGHT = 1
};

class RiftApp : public App {
public:
    GL::FrameBuffer buffer;
    GL::Shaders::Program distortProgram;
    Stereo::StereoConfig sconfig;
    RiftApp(int x = 0, int y = 0, int width = 480, int height = 300);
    virtual ~RiftApp() {
    }

    virtual void renderScene(Eye e) = 0;
    virtual void init();
    void display();
    void initPostProcess(Eye eye);
};
}
