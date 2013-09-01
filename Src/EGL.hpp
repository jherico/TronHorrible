#pragma once
#include "Common.hpp"
#include <EGL/egl.h>
#include <string>
class EGL {
public:
    /// Window handle
    EGLNativeWindowType  hWnd;
    const EGLDisplay  display;
    EGLContext  context;
    EGLSurface  surface;

    EGLNativeWindowType createNativeWindow(int w, int h, int x, int y);

    EGLConfig init(EGLint * attribList);
public:
   EGL();
   void createWindow(int w = 1280, int h = 800, int x = 0, int y = 0);
   void surfaceless();
   void swap();
   void makeCurrent();
//   void setViewport();
//   void setViewport(float, float, float, float);
};


