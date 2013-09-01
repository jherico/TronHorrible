#include "EGL.hpp"
#include "Program.hpp"
#include <time.h>
#include <iostream>

using namespace std;

#define PH 0.5
#define PQ 0.25
#define NH -0.5
#define NO -0.5
#define ON 0.5
#define ZE 0

void bindNextFrame(GLuint texture);
void init_gl(GLint ploc, GLint tloc);
void render_gl();

class App {
//    VideoCapture videoCapture;
    Program program;
    GLuint imageTexture;
//    VertexBuffer vertices;
//    IndexBuffer indices;
    GLuint vertexAttribute;
    GLuint textureAttribute;
    EGL egl;
    float xoffset, yoffset;
    float aspect;
    float zoom;

protected:
    int x, y;
    int width, height;

public:

    App(int x = 0, int y = 0, int width = 1280, int height = 800)
            : x(x), y(y), width(width), height(height), //
            program("test") {
        egl.createWindow(width, height, x, y);
//        indices = IndexBuffer(GL_TRIANGLES, { 0, 1, 2, 2, 3, 0, }), //
//        vertices = VertexBuffer(VERTEX_HAS_TEX, { //
//                Vertex::simpleTextured(glm::vec3(NO, NO, ZE), glm::vec2(ZE, ZE)), //
//                Vertex::simpleTextured(glm::vec3(ON, NO, ZE), glm::vec2(ON, ZE)), //
//                Vertex::simpleTextured(glm::vec3(ON, ON, ZE), glm::vec2(ON, ON)), //
//                Vertex::simpleTextured(glm::vec3(NO, ON, ZE), glm::vec2(ZE, ON)) //
//                });
    }

    virtual ~App() {

    }

    void run() {
        init();
        unsigned int frames = 0;
        int i = 0;
        time_t t1 = time(0);
        while (processEvents()) {
            // Make the context current
//            egl.makeCurrent();
            update();
            display();
            egl.swap();
            frames++;
            time_t t2 = time(0);
            if (t2 - t1 >= 2.0f) {
                t1 = t2;
                cerr << frames << " frames rendered in 2 second -> FPS=" << (float)frames/ 2.0f << endl;
                frames = 0;
            }
//            if (++i > 100) {
//                break;
//            }
        }
    }

    virtual bool keys(unsigned char key, int x, int y) {
        switch (key) {
        case 27:
        case 'q':
            return false;
            break;
        }
        return true;
    }

    virtual void resize(int width, int height) {
        this->width = width;
        this->height = height;
    }

    virtual bool processEvents() {
        return true;
    }

    virtual void init() {
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.5, 0.5, 0.0, 1.0);
        glGenTextures(1, &imageTexture);
        glBindTexture(GL_TEXTURE_2D, imageTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glViewport ( 0, 0, 1280, 800);
        program.use();
        textureAttribute = program.getLocation(ATTRIBUTE, "in_texture");
        vertexAttribute = program.getLocation(ATTRIBUTE, "in_position");
        init_gl(vertexAttribute, textureAttribute);
//        vertices.bind(vertexAttribute, textureAttribute);
//        indices.bind();
//        glUseProgram(0);
    }

    virtual void update() {
        bindNextFrame(imageTexture);
        if (program.link()) {
            program.use();
        }
    }

    virtual void display() {
        render_gl();
    }
};

void run_app() {
        (new App(0, 0, 1280, 800))->run();
}
