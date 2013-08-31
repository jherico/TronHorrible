#include "EGL.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <exception>

using namespace std;

EGLConfig EGL::init(EGLint * attribList) {
    EGLConfig config;
    // Choose config
    int numConfig;
    if (!eglChooseConfig(display, attribList, &config, 1, &numConfig)) {
        throw exception();
    }

    // Create a GL context
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        throw exception();
    }
    return config;
}

EGL::EGL()
        : hWnd(0), display(eglGetDisplay(EGL_DEFAULT_DISPLAY)), context(EGL_NO_CONTEXT), surface(EGL_NO_SURFACE) {
    // Get Display
    if (display == EGL_NO_DISPLAY) {
        throw exception();
    }

    // Initialize EGL
    EGLint majorVersion;
    EGLint minorVersion;
    if (!eglInitialize(display, &majorVersion, &minorVersion)) {
        throw exception();
    }

}

void EGL::createWindow(int w, int h, int x, int y) {
    EGLint attribList[] = {
    EGL_RED_SIZE,
                            1,
                            EGL_GREEN_SIZE,
                            1,
                            EGL_BLUE_SIZE,
                            1,
                            EGL_DEPTH_SIZE,
                            16,
                            EGL_RENDERABLE_TYPE,
                            EGL_OPENGL_ES2_BIT,
                            EGL_NONE };

    EGLConfig config = init(attribList);
    hWnd = createNativeWindow(w, h, x, y);

    // Create a surface
    surface = eglCreateWindowSurface(display, config, hWnd, NULL);
    if (surface == EGL_NO_SURFACE) {
        throw exception();
    }

    // Make the context current
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw exception();
    }
}

void EGL::surfaceless() {
    int rgba_attribs[] = {
    EGL_RED_SIZE,
                           1,
                           EGL_GREEN_SIZE,
                           1,
                           EGL_BLUE_SIZE,
                           1,
                           EGL_RENDERABLE_TYPE,
                           EGL_OPENGL_ES2_BIT,
                           EGL_NONE };
    EGLConfig config = init(rgba_attribs);
    const char * exts = eglQueryString(display, EGL_EXTENSIONS);
    if (!strstr(exts, "EGL_KHR_surfaceless_opengl")) {
        throw exception();
    }
}

void EGL::swap() {
    if (!eglSwapBuffers(display, surface))
        throw exception();
}

void EGL::makeCurrent() {
    if (!eglMakeCurrent(display, surface, surface, context))
        throw exception();
}

