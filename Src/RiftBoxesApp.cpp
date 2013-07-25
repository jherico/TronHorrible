#include "RiftApp.hpp"


using namespace std;
using namespace boost;
using namespace Encom13;
using namespace Encom13::GL;
using namespace Encom13::GL::Shaders;
using namespace Encom13::Stereo;
using namespace Encom13::Blas;

class RiftBoxesApp : public RiftApp {
public:
    RiftBoxesApp(int x = 0, int y = 0, int width = 480, int height = 300)
            : RiftApp(x, y, width, height) { }

    void init() {
        RiftApp::init();
    }


    void renderScene(Eye eye) {
        glClearColor(0.0, 0.0, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        Program::getProgram("colored").get()->use();
        Program::setActiveProjection(glm::perspective<float>(110.0, 0.8 * 2, 0.01, 1000.0));
        Program::setActiveModelview(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
        Utils::renderWireColorCube();

        Program::setActiveModelview(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)));
        Utils::renderWireColorCube();

        Program::setActiveModelview(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)));
        Utils::renderWireColorCube();
    }

    void update(float time) {
    }
};

int rift_boxes_main(int argc, char **argv) {
//    (new RiftVideoApp(1920, 1080 * 2 - 800, 1280, 800))->run();
    (new RiftBoxesApp(1920, 0, 1280, 800))->run();
//    (new RiftVideoApp(0, 0, 1280, 800))->run();
    return 0;
}
