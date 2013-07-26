#include "RiftApp.hpp"

using namespace Encom13::Stereo;

using namespace Encom13::GL::Shaders;

//#define SKIP_MULTIPASS 1

namespace Encom13 {

RiftApp::RiftApp(int x, int y, int width, int height)
        : App(x, y, width, height), distortProgram(*Program::getProgram("distort").get()) {
}

void RiftApp::init() {
    setViewport();
    // Allocate the frame buffer at twice the rift resolution
    buffer.init(1280, 1600);
    buffer.activate();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.5, 0.0, 1.0);
    buffer.deactivate();
//    distortedRenderer.init();
}

void RiftApp::display() {
    setViewport();
    glClearColor(0.0, 0.5, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef SKIP_MULTIPASS
    for (int i = 0; i < 2; ++i) {
        Eye eye = (Eye) i;
        buffer.activate();
        renderScene(eye);
        buffer.deactivate();

        initPostProcess(eye);
        GL::Utils::renderFullscreenTexture(buffer.getTexture());
    }
#else
    renderScene(LEFT);
#endif

}

void RiftApp::initPostProcess(Eye eye) {
    float as = (float(width) / float(height)) / 2.0;
    const HMDInfo & info = sconfig.GetHMDInfo();
    float lensOffset = (info.LensSeparationDistance / info.HScreenSize);
    if (eye == RIGHT) {
        setViewport(0.5, 0, 0.5, 1);
    } else {
        lensOffset = 1 - lensOffset;
        setViewport(0, 0, 0.5, 1);
    }
    distortProgram.use();
    distortProgram.setUniform2f("LensCenter", lensOffset, 0.5);
    distortProgram.setUniform1f("AspectRatio", as);
    const DistortionConfig & dk = sconfig.GetDistortionConfig();
    float K0 = dk.K[0];
    float K1 = dk.K[1];
    float K2 = dk.K[2];
    float K3 = dk.K[3];
    distortProgram.setUniform4f("HmdWarpParam", K0, K1, K2, K3);
}

}
