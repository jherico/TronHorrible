#include "App.hpp"

#include <math.h>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <opencv2/opencv.hpp>
#include <algorithm>

#include "EGL.hpp"
#include "Stereo.hpp"

using namespace std;
using namespace cv;
using namespace boost;
using namespace Encom13::GL;
using namespace Encom13::Stereo;
using namespace Encom13::Blas;

namespace Encom13 {

    void App::run() {
        init();
        timeval t1;
        struct timezone tz;
        gettimeofday(&t1, &tz);

        float totaltime = 0.0f;
        unsigned int frames = 0;

        while (processEvents()) {
            timeval t2;
            gettimeofday(&t2, &tz);
            float deltatime = (float) (t2.tv_sec - t1.tv_sec
                    + (t2.tv_usec - t1.tv_usec) * 1e-6);
            t1 = t2;

            // Make the context current
            egl.makeCurrent();
            update(deltatime);
            display();
            egl.swap();

            totaltime += deltatime;
            frames++;
            if (totaltime > 2.0f) {
                printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n",
                        frames, totaltime, frames / totaltime);
                totaltime -= 2.0f;
                frames = 0;
            }
        }
    }

    void App::resize(int width, int height) {
        this->width = width;
        this->height = height;
    }

    bool App::keys(unsigned char key, int x, int y) {
        switch (key) {
        case 27:
        case 'q':
            return false;
            break;
        }
        return true;
    }

    void App::setViewport() {
        glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    }

    void App::setViewport(float xp, float yp, float wp, float hp) {
        glViewport(xp * width, yp * height, wp * width, hp * height);
    }

}
