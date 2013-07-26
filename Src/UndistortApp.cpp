
#include "common.hpp"
#include "App.hpp"
#include <OVR.h>

#include "SensorFusion.hpp"

using namespace std;
using namespace boost;
using namespace cv;
using namespace Encom13;
using namespace Encom13::GL;
using namespace Encom13::GL::Shaders;

class UndistortApp : public Encom13::App {
    VideoCapture capture;
    GLuint texture;

public:
    UndistortApp(int x = 0, int y = 0, int width = 1280, int height = 800) :
        Encom13::App(x, y, width, height) {
    }

    virtual ~UndistortApp() {
    }

    virtual void init() {
        capture = VideoCapture(0);
        capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    void update(float time) {
        if (!capture.grab()) {
            throw std::exception();
        }

        static Mat frame;
        capture.retrieve(frame);
        cv::flip(frame, frame, 0);
        cvtColor(frame, frame, CV_BGR2RGB);
        bindImage(frame, texture);
    }

    static void bindImage(Mat image, int texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    }


    void display() {
        glClearColor(0.0, 0.0, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Program::getProgram("undistort").get()->use();
        Utils::renderFullscreenTexture(texture);
    }
};

int undistort_gl_app(int argc, char ** argv) {
    (new UndistortApp(0, 0, 1280, 800))->run();
}
