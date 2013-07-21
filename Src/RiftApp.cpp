#include "RiftApp.hpp"

using namespace Encom13::Stereo;

namespace Encom13 {

RiftApp::RiftApp(int x, int y, int width, int height)
        : App(x, y, width, height), distortProgram("shaders/distort.vs", "shaders/distort.fs") {
}

void RiftApp::init() {
    window.setViewport();
    // Allocate the frame buffer at twice the rift resolution
    buffer.init(1280, 1600);
    buffer.activate();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.5, 0.0, 1.0);
    buffer.deactivate();
}

void RiftApp::display() {
    window.setViewport();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef SKIP_MULTIPASS
    for (int i = 0; i < 2; ++i) {
        Eye eye = (Eye) i;
        buffer.activate();
        renderScene(eye);
        buffer.deactivate();

        distortProgram.use();
        initPostProcess(eye);
        Encom13::GL::Utils::renderFullscreenTexture(buffer.getTexture());
    }
#else
    renderScene(LEFT);
#endif

}

void RiftApp::initPostProcess(Eye eye) {
    float as = (float(window.width) / float(window.height)) / 2.0;
    const HMDInfo & info = sconfig.GetHMDInfo();
    float lensOffset = (info.LensSeparationDistance / info.HScreenSize);
    if (eye == RIGHT) {
        window.setViewport(0.5, 0, 0.5, 1);
    } else {
        lensOffset = 1 - lensOffset;
        window.setViewport(0, 0, 0.5, 1);
    }
    distortProgram.setUniform2f("LensCenter", lensOffset, 0.5);
    distortProgram.setUniform1f("AspectRatio", as);
    const float * K = sconfig.GetDistortionConfig().K;
    distortProgram.setUniform4f("HmdWarpParam", K[0], K[1], K[2], K[3]);
}

}
