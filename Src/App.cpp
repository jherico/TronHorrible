#include "EGL.hpp"
#include "Geometry.hpp"
#include "Program.hpp"

using namespace std;
using namespace cv;
using namespace boost;

#define PH 0.5
#define PQ 0.25
#define NH -0.5
#define NO -1
#define ON 1
#define ZE 0

class App {
    VideoCapture videoCapture;
    Program program;
    GLuint imageTexture;
    VertexBuffer vertices;
    IndexBuffer indices;
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
            program("tronHorrible") {
        cerr << "App ctor" << endl;
        egl.createWindow(width, height, x, y);
        indices = IndexBuffer(GL_TRIANGLES, { 0, 1, 2, 2, 3, 0, }), //
        vertices = VertexBuffer(VERTEX_HAS_TEX, { //
                Vertex::simpleTextured(glm::vec3(NO, NO, ZE), glm::vec2(ZE, ZE)), //
                Vertex::simpleTextured(glm::vec3(ON, NO, ZE), glm::vec2(ON, ZE)), //
                Vertex::simpleTextured(glm::vec3(ON, ON, ZE), glm::vec2(ON, ON)), //
                Vertex::simpleTextured(glm::vec3(NO, ON, ZE), glm::vec2(ZE, ON)) //
                });
    }

    virtual ~App() {

    }

    void run() {
        cerr << "run pre-init" << endl;
        init();
        cerr << "run post-init" << endl;
        timeval t1;
        struct timezone tz;
        gettimeofday(&t1, &tz);

        float totaltime = 0.0f;
        unsigned int frames = 0;

        while (processEvents()) {
            timeval t2;
            gettimeofday(&t2, &tz);
            float deltatime = (float) (t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
            t1 = t2;

            // Make the context current
            egl.makeCurrent();
            update(deltatime);
            display();
            egl.swap();

            totaltime += deltatime;
            frames++;
            if (totaltime > 2.0f) {
                printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames / totaltime);
                totaltime -= 2.0f;
                frames = 0;
            }
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
        glBindTexture(GL_TEXTURE_2D, 0);
        cerr << "init pre-video" << endl;
        videoCapture = VideoCapture(0);
        videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
        videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
        cerr << "init pre-program" << endl;
        program.use();
        textureAttribute = program.getLocation(ATTRIBUTE, "in_texture");
        vertexAttribute = program.getLocation(ATTRIBUTE, "in_position");
        glUseProgram(0);
    }

    virtual void update(float time) {
        if (!videoCapture.grab()) {
            return;
        }
        Mat frame(1280, 720, CV_32S);
        videoCapture.retrieve(frame);
        cv::flip(frame, frame, 0);
        cvtColor(frame, frame, CV_BGR2RGB);
        glBindTexture(GL_TEXTURE_2D, imageTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
    }

    virtual void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.use();
        vertices.bind(vertexAttribute, textureAttribute);
        indices.bind();
        indices.render();
        GLenum err = glGetError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

int main(int argc, char **argv) {
    cerr << "Main" << endl;
    try {
        (new App(0, 232, 1280, 800))->run();

    } catch (std::string & err) {
        cerr << "Caught exception" << endl;
        cerr << err;
        return -1;
    }
    return 0;
}
