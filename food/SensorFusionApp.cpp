
#include "common.hpp"
#include "App.hpp"
//#include <OVR.h>

#include "SensorFusion.hpp"

using namespace Encom13::GL;
using namespace Encom13::GL::Shaders;

class SensorFusionApp ;

SensorFusionApp * pApp;

class SensorFusionApp : public Encom13::App {
public:

    SensorFusion sensorFusion;

    glm::mat4 camera;
    glm::mat4 view;
    glm::quat orientation;


    SensorFusionApp(int x = 0, int y = 0, int width = 480, int height = 300) : Encom13::App(x, y, width, height) {
        view = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        camera = glm::inverse(view);
    }

    virtual ~SensorFusionApp() {
    }

    static void callback(const OvrSensorMessage * message) {
        pApp->callbackMember(*message);
    }

    void callbackMember(const OvrSensorMessage & message) {
        sensorFusion.processTrackerData(message);
    }

    virtual void init() {
        pApp = this;
//        OVR_HANDLE handle = ovrOpenRiftRecording("../OculusSDK/Samples/data/rift1.json");
//        ovrRegisterSampleHandler(handle, &SensorFusionApp::callback);
    }

    virtual void update(float time) {
        static glm::mat4 rot = glm::rotate(glm::mat4(), (float)3.14159 / (float)20, glm::vec3(0, 1, 0));
        static glm::quat qrot = glm::rotate(glm::quat(), (float)3.14159 / (float)20, glm::vec3(0, 1, 0));
        orientation = sensorFusion.GetOrientation();
     //   orientation = orientation * glm::angleAxis(time * 10, glm::vec3(0, 1, 1));
    }

    void display() {
        glClearColor(0.0, 0.0, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glLineWidth(3.0);
        Program::getProgram("colored").get()->use();
        Program::setActiveProjection(glm::perspective<float>(110.0, 0.8 * 2, 0.01, 1000.0));
        Program::setActiveModelview(view * glm::mat4_cast(orientation));
        Utils::renderOrigin();
//        VertexBuffer vertexData(VERTEX_HAS_COLOR, {
//            Vertex::simpleColored(glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5)),
//            Vertex::simpleColored(pos, glm::vec3(1, 1, 1)),
//        });
//        IndexBuffer indexData(GL_LINES, { 0, 1 });
//        vertexData.bind();
//        vertexData.setAttributes();
//        indexData.bind();
//        indexData.render();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
};

int sensor_fusion_main(int argc, char ** argv) {
    (new SensorFusionApp(0, 0, 1280, 800))->run();
}
