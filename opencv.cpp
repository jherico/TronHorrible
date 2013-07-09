#include <math.h>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>
#include "Common/esUtil.h"
#include "common.hpp"
//#include "ds.hpp"
#include "hand.hpp"
#include <algorithm>

using namespace std;
using namespace cv;
//using namespace Encom13::Input::Softkinetic;

#define VIDEO_FILE "/mnt/internal/Tron Legacy (2010).avi"

void display(ESContext* context);
void update(ESContext*, float);
void keys(ESContext*, unsigned char, int, int);

#define NUM_FINGERS 5
#define NUM_DEFECTS 8

GLfloat vertices[] = { -1, -1, 0, 1, 0, 1, 0, 1,
                        1, -1, 0, 1, 1, 1, 0, 1,
                        1,  1, 0, 1, 1, 0, 0, 1,
                       -1,  1, 0, 1, 0, 0, 0, 1, };

GLubyte indices[] = { 0, 1, 2, 2, 3, 0, };

class App {
public:
    typedef boost::shared_ptr<GL::ES::Program> ProgramPtr;
    GLuint texture;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    ESContext esContext;
    ProgramPtr program;
    GLint vloc;
    GLint tloc;

    VideoCapture capture;
    Mat frame;
    Mat thr_image;
    vector<Point> contour;
    bool contourFound;
    Point hand_center;

    Mat kernel; /* Kernel for morph operations */

    int num_fingers;
    int hand_radius;
    int num_defects;

    App() {
        esInitContext(&esContext);
        esContext.userData = this;

        initUi();
//        initGl();
        initCv();
    }

    void run() {
        while ('q' != cvWaitKey(1)) {
                update(1);
                usleep(1000);
        }

//        esMainLoop(&esContext);
    }

    void initCv() {
        //capture = cvCreateFileCapture(VIDEO_FILE);
        capture = VideoCapture(0);
        capture.retrieve(frame);

        //IplImage *image = cvLoadImage("/usr/share/backgrounds/gnome/FootFall.png");
        //texture = ConvertIplToTexture(image);
        //cvReleaseImage(&image);
    }

    void initUi() {
        cvNamedWindow("output", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("thresholded", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("output", 50, 50);
        cvMoveWindow("thresholded", 700, 50);

//        esCreateWindow(&esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB, 100, -700);
//        esRegisterDrawFunc(&esContext, ::display);
//        esRegisterUpdateFunc(&esContext, ::update);
//        esRegisterKeyFunc(&esContext, ::keys);

    }

    void initGl() {
        glViewport(0, 0, (GLsizei) esContext.width, (GLsizei) esContext.height);

        program = ProgramPtr(new GL::ES::Program("shaders/textured.vs", "shaders/textured.fs"));
        vloc = glGetAttribLocation(program.get()->program, "a_position");
        tloc = glGetAttribLocation(program.get()->program, "a_texCoord");

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        int size = sizeof(vertices);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        size = sizeof(indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_DYNAMIC_DRAW);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.5, 1.0);
    }

    void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the program object
        glUseProgram(program.get()->program);

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glEnableVertexAttribArray(vloc);
        glEnableVertexAttribArray(tloc);

        // Load the vertex position
        int stride = sizeof(GLfloat) * 8;
        glVertexAttribPointer(vloc, 4, GL_FLOAT, GL_FALSE, stride, 0);

        // Load the texture coordinate
        glVertexAttribPointer(tloc, 4, GL_FLOAT, GL_FALSE, stride, (void*) (sizeof(GLfloat) * 4));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*) 0);

    }

    void update(float time) {
        capture.grab();
        bool b = capture.retrieve(frame);
        imshow("output", frame);

        Encom13::Gesture::Hand hand;
        hand.filter_and_threshold(frame, thr_image);
        hand.find_contour(thr_image);
        hand.find_convex_hull();
//        find_fingers();
        hand.annotate(frame);

//        cvShowImage("thresholded", thr_image);
        //usleep(100000/24);
        imshow("output", frame);
//        imshow("thresholded", thr_image);

        cvtColor(frame, frame, CV_BGR2RGB);
        bindImage(frame);
    }

    void bindImage(Mat image) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void keys(unsigned char key, int x, int y) {
        switch (key) {
        case 27:
        case 'q':
            exit(0);
            break;
        }
    }

};


void display(ESContext* context) {
    ((App*) context->userData)->display();
}

void update(ESContext* context, float time) {
    ((App*) context->userData)->update(time);
}

void keys(ESContext* context, unsigned char a, int b, int c) {
    ((App*) context->userData)->keys(a, b, c);
}

int main(int argc, char **argv) {
    cout << "Starting" << endl;
    try {
        (new App())->run();
    } catch (string * err) {
        const char * errStr = err->c_str();
        cerr << errStr << endl;

    }
    cout << "done" << endl;
}


//
//double min, max;
//cv::minMaxIdx(g_depthImage, &min, &max);
//cv::Mat adjMap;
//cv::convertScaleAbs(g_depthImage, adjMap, 255 / (max - min));
//Mat thr_image;
//inRange(g_depthImage, Scalar(th_min), Scalar(th_max), thr_image);
//
//cv::Mat falseColorsMap;
//
///* OpenCV display - this will slow stuff down, should be in thread*/
//
//imshow("color", thr_image);
//imshow("depth", adjMap);
//
//// Allow OpenCV to shut down the programadjMap

//            g_context.run();
//            g_context.stopNodes();
