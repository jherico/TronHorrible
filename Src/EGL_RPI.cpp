//#include "App.hpp"
#include "EGL.hpp"

#include <memory.h>
#define TRUE 1
#define FALSE 0

using namespace std;

//    bool App::processEvents() {
//        XEvent xev;
//        KeySym key;
//        bool exit = false;
//        char text;
//
//        // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
//        while (XPending(x_display)) {
//            XNextEvent(x_display, &xev);
//            if (xev.type == KeyPress) {
//                if (XLookupString(&xev.xkey, &text, 1, &key, 0) == 1) {
//                    if (!keys(text, 0, 0)) {
//                        exit = true;
//                    }
//                }
//            }
//            if (xev.type == ConfigureNotify) {
//                XConfigureEvent xce = xev.xconfigure;
//                if (xce.width != width || xce.height != height) {
//                    resize(xce.width, xce.height);
//                }
//            }
//            if (xev.type == DestroyNotify)
//                exit = true;
//        }
//        return !exit;
//    }
//
//
//


EGLNativeWindowType EGL::createNativeWindow(int width, int height, int x, int y) {
    return (EGLNativeWindowType) 0;
}

