#pragma once
#include "common.hpp"
#include "EGL.hpp"

//#include <math.h>
//#include <iostream>
//#include <boost/smart_ptr.hpp>
//#include <boost/function.hpp>
//#include <boost/bind.hpp>
//#include <opencv2/opencv.hpp>
//#include <algorithm>
//
//#include "FrameBuffer.hpp"
//#include "Program.hpp"
//#include "Stereo.hpp"

namespace Encom13 {

    class App {
    public:
        GL::EGL window;

        App(int x = 0, int y = 0, int width = 16 * 30, int height = 10 * 30) :
                window(width, height, x, y) {
            window.createWindow();
        }
        virtual ~App() {

        }

        virtual void run();
        virtual bool processEvents();
        virtual void init() = 0;
        virtual void resize(int width, int height);
        virtual void display() = 0;
        virtual void update(float time) = 0;
        virtual bool keys(unsigned char key, int x, int y);
    };
}