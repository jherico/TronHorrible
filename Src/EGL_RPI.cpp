//#include "App.hpp"
#include "EGL.hpp"

#include <iostream>
#include <string>
#include <memory.h>
#include <bcm_host.h>
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

    uint32_t screen_width = 1280, screen_height = 800;
    // create an EGL window surface
//    int32_t success = graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);
    //assert( success >= 0 );
    cerr << screen_width << endl;
    cerr << screen_height << endl;

    VC_RECT_T dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = screen_width;
    dst_rect.height = screen_height;

    VC_RECT_T src_rect;
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = screen_width << 16;
    src_rect.height = screen_height << 16;

    DISPMANX_DISPLAY_HANDLE_T dispman_display = vc_dispmanx_display_open( 0 );

    DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start( 0 );
    DISPMANX_ELEMENT_HANDLE_T dispman_element = vc_dispmanx_element_add(
            dispman_update, dispman_display,
            0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, DISPMANX_NO_ROTATE);

    static EGL_DISPMANX_WINDOW_T nativewindow;
    nativewindow.element = dispman_element;
    nativewindow.width = screen_width;
    nativewindow.height = screen_height;

    vc_dispmanx_update_submit_sync( dispman_update );

    return &nativewindow;
}

