#include "EGL.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

using namespace std;

EGLConfig EGL::init(EGLint * attribList) {
    EGLConfig config;
    // Choose config
    int numConfig;
    if (!eglChooseConfig(display, attribList, &config, 1, &numConfig)) {
        throw string("Failed to choose config");
    }

    if (EGL_FALSE == eglBindAPI(EGL_OPENGL_ES_API)) {
        throw string("Failed to bind OpenGL ES");
    }

    // Create a GL context
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        throw string("Failed to create context");
    }
    return config;
}

EGL::EGL()
        : hWnd(0), display(eglGetDisplay(EGL_DEFAULT_DISPLAY)), context(EGL_NO_CONTEXT), surface(EGL_NO_SURFACE) {
    // Get Display
    if (display == EGL_NO_DISPLAY) {
        throw string("Failed to get display");
    }

    // Initialize EGL
    EGLint majorVersion;
    EGLint minorVersion;
    if (!eglInitialize(display, &majorVersion, &minorVersion)) {
        throw string("Failed to init EGL");
    }

}

void EGL::createWindow(int w, int h, int x, int y) {
    EGLint attribList[] = {
                           EGL_RED_SIZE, 8,
                           EGL_GREEN_SIZE, 8,
                           EGL_BLUE_SIZE, 8,
                           EGL_ALPHA_SIZE, 8,
                           EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
//                           EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                           EGL_NONE
                        };

    EGLConfig config = init(attribList);
    cerr << config << endl;
    hWnd = createNativeWindow(w, h, x, y);

    // Create a surface
    surface = eglCreateWindowSurface(display, config, hWnd, NULL);
    if (surface == EGL_NO_SURFACE) {
        throw string("Failed to create surface");
    }

    // Make the context current
    makeCurrent();
    // Set background color and clear buffers
    glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT );

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
        throw string("Failed to find EGL_KHR_surfaceless_opengl extension");
    }
}

void EGL::swap() {
    if (!eglSwapBuffers(display, surface))
        throw string("Failed to swap buffers");
}

void EGL::makeCurrent() {
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw string("Failed to activate egl context");
    }
}

