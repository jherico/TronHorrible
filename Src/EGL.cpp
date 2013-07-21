#include "EGL.hpp"
#include "common.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <exception>
#include "encomGL.hpp"

using namespace std;

namespace Encom13 {
    namespace GL {
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


        EGL::EGL(int width, int height, int x, int y) :
                hWnd(0), display(eglGetDisplay(EGL_DEFAULT_DISPLAY)), context(EGL_NO_CONTEXT), surface(EGL_NO_SURFACE),
                width(width), height(height), x(x), y(y) {
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

        void EGL::createWindow() {
            EGLint attribList[] = {
                    EGL_RED_SIZE, 1,
                    EGL_GREEN_SIZE, 1,
                    EGL_BLUE_SIZE, 1,
                    EGL_DEPTH_SIZE, 16,
                    // EGL_ALPHA_SIZE, 8,
                    EGL_NONE };

            EGLConfig config = init(attribList);
            hWnd = createNativeWindow();

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
                    EGL_RED_SIZE, 1,
                    EGL_GREEN_SIZE, 1,
                    EGL_BLUE_SIZE, 1,
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL_NONE
            };
            EGLConfig config = init(rgba_attribs);
            const char * exts = eglQueryString(display, EGL_EXTENSIONS);
            if (!strstr(exts, "EGL_KHR_surfaceless_opengl")) {
                throw exception();
            }
        }


        void EGL::createPixmap() {
            EGLint attribList[] = {
                    EGL_RED_SIZE, 1,
                    EGL_GREEN_SIZE, 1,
                    EGL_BLUE_SIZE, 1,
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
                    EGL_NONE };

            EGLConfig config = init(attribList);

            hWnd = createNativePixmap();

            surface = eglCreatePixmapSurface(display, config, hWnd, NULL);
            if (surface == EGL_NO_SURFACE) {
                EGLint err = eglGetError();
                if (err == EGL_BAD_ALLOC) {
                    cerr << "bad alloc" << endl;
                }
                throw gl_exception();
            }
//            EGLint pbuf_attribs[5];
//            pbuf_attribs[0] = EGL_WIDTH;
//            pbuf_attribs[1] = width;
//            pbuf_attribs[2] = EGL_HEIGHT;
//            pbuf_attribs[3] = height;
//            pbuf_attribs[4] = EGL_NONE;
//            surface = eglCreatePbufferSurface(display, config, pbuf_attribs);
//
//            EGLint attribList[] = {
//                    EGL_HEIGHT, height,
//                    EGL_WIDTH, width,
//                    // EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB,
//                    // EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
//                    // EGL_BLUE_SIZE, 1,
//                    // EGL_DEPTH_SIZE, 16,
//                    // EGL_ALPHA_SIZE, 8,
//                    EGL_NONE };
//
//            // Create a surface
//            surface = eglCreatePbufferSurface(display, config, attribList);
//            if (surface == EGL_NO_SURFACE) {
//                EGLint err = eglGetError();
//                if (err == EGL_BAD_ALLOC) {
//                    cerr << "bad alloc" << endl;
//                }
//                throw gl_exception();
//            }

            // Make the context current
//            makeCurrent();
//            glGenTextures(1, &surfaceTexture);
//            glBindTexture(GL_TEXTURE_2D, surfaceTexture);
//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        void EGL::swap() {
            if (!eglSwapBuffers(display, surface))
                throw exception();
        }

        void EGL::makeCurrent() {
            if (!eglMakeCurrent(display, surface, surface, context))
                throw exception();
        }

        void EGL::setViewport() {
            glViewport(0, 0, (GLsizei) width, (GLsizei) height);
        }

        void EGL::setViewport(float xp, float yp, float wp, float hp) {
            glViewport(xp * width, yp * height, wp * width, hp * height);
        }

        void EGL::resize(int width, int height) {
            this->width = width;
            this->height = height;
        }

    }
}
