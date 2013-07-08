#include <math.h>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>
#include "Common/esUtil.h"
#include "common.hpp"
#include "ds.hpp"
#include <algorithm>

using namespace std;
using namespace cv;
using namespace Encom13::Input::Softkinetic;

#define VIDEO_FILE "/mnt/internal/Tron Legacy (2010).avi"

void display(ESContext* context);
void update(ESContext*, float);
void keys(ESContext*, unsigned char, int, int);

#define NUM_FINGERS 5
#define NUM_DEFECTS 8

GLfloat vertices[] = { -1, -1, 0, 1, 0, 1, 0, 1,

1,
                       -1, 0, 1, 1, 1, 0, 1,

                       1,
                       1, 0, 1, 1, 0, 0, 1,

                       -1,
                       1, 0, 1, 0, 0, 0, 1, };

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
        initGl();
        initCv();
    }

    void run() {
        esMainLoop(&esContext);
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

        esCreateWindow(&esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB, 100, -700);
        esRegisterDrawFunc(&esContext, ::display);
        esRegisterUpdateFunc(&esContext, ::update);
        esRegisterKeyFunc(&esContext, ::keys);

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

//        filter_and_threshold();
//        find_contour();
//        find_convex_hull();
//        find_fingers();

//        annotate();

//        cvShowImage("thresholded", thr_image);
        //usleep(100000/24);
        imshow("output", frame);
        imshow("thresholded", thr_image);

        cvtColor(frame, frame, CV_BGR2RGB);
        bindImage(frame);
        cvWaitKey(1);
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

void filter_and_threshold(const Mat & input, Mat & output) {
    Mat temp_image3;
    static Mat kernel = getStructuringElement(CV_SHAPE_RECT, Size(9, 9), Point(4, 4));
    GaussianBlur(input, temp_image3, Size(11, 11), 0);
    medianBlur(temp_image3, temp_image3, 11);
    cvtColor(temp_image3, temp_image3, CV_BGR2HSV);
    inRange(temp_image3, Scalar(0, 0, 160, 255), Scalar(255, 400, 300, 0), output);
    /* Apply morphological opening */
    morphologyEx(output, output, MORPH_OPEN, kernel);
    GaussianBlur(output, output, Size(3, 3), 0);
}

struct Hand {
    vector<Point> contour;
    vector<Point> curve;
    int radius;
    Point center;
    vector<Point> defects;
    vector<Point> fingers;
    vector<int> finderDists;
};

bool find_contour(const Mat & input, Hand & hand) {
    Mat temp_image1 = input.clone();

    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //! retrieves contours and the hierarchical information from black-n-white image.
    findContours(temp_image1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    double max_area = 0.0;
    int found = -1;
    bool contourFound = false;
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = fabs(contourArea(contours[i]));
        if (area > max_area) {
            found = i;
        }
    }

    if (-1 != found) {
        hand.contour = contours[found];
        approxPolyDP(hand.contour, hand.curve, 2, true);
        hand.contour = hand.curve;
        contourFound = true;
    }
    return contourFound;
}

void find_convex_hull(Hand & hand) {
    vector<int> hull;
    vector<Vec4i> defects;
    convexHull(hand.contour, hull);
    //We calculate convexity defects
    convexityDefects(hand.contour, hull, defects);
    int x = 0, y = 0;
    int dist = 0;

    if (!defects.empty()) {
        for (size_t cDefIt = 0; cDefIt < defects.size(); cDefIt++) {
            int defectPtIdx = defects[cDefIt].val[2];
            // do something with **.. This is e.g. the defect point.
            Point p = hand.contour.at(defectPtIdx);
            x += p.x;
            y += p.y;
            hand.defects.push_back(p);
        }
        int num_defects = defects.size();
        x /= num_defects;
        y /= num_defects;
        hand.center = Point(x, y);
        for (size_t cDefIt = 0; cDefIt < defects.size(); cDefIt++) {
            int defectPtIdx = defects[cDefIt].val[2];
            Point dfct = hand.contour.at(defectPtIdx);
            Point diff = hand.center;

            diff.x -= dfct.x;
            diff.y -= dfct.y;
            diff.x *= diff.x;
            diff.y *= diff.y;
            dist += sqrt(diff.x + diff.y);
        }
        hand.radius = num_defects;
    }
}

void find_fingers(Mat & image, Hand & hand) {
    int num_fingers = 0;
    if (hand.defects.empty()) {
        return;
    }

    /*
     * Fingers are detected as points where the distance to the center
     * is a local maximum
     */
    Point max_point;
    int dist1 = 0, dist2 = 0;
    for (int i = 0; i < hand.contour.size(); i++) {
        const Point & c = hand.center;
        Point d = hand.contour.at(i);
        d.x -= c.x;
        d.y -= c.y;
        d.x *= d.x;
        d.y *= d.y;
        int dist = d.x + d.y;

        if (dist < dist1 && dist1 > dist2 && max_point.x != 0 && max_point.y < image.rows - 10) {
            hand.finderDists.push_back(dist);
            hand.fingers.push_back(max_point);
            if (num_fingers >= 6)
                break;
        }

        dist2 = dist1;
        dist1 = dist;
        max_point = hand.contour.at(i);
    }
}

void display(ESContext* context) {
    ((App*) context->userData)->display();
}

void update(ESContext* context, float time) {
    ((App*) context->userData)->update(time);
}

void keys(ESContext* context, unsigned char a, int b, int c) {
    ((App*) context->userData)->keys(a, b, c);
}

int main3(int argc, char **argv) {
    cout << "Starting" << endl;
    try {
        (new App())->run();
    } catch (string * err) {
        const char * errStr = err->c_str();
        cerr << errStr << endl;

    }
    cout << "done" << endl;
}

void annotate(Mat & image, const Hand & hand) {
//    CV_EXPORTS_W void drawContours( InputOutputArray image, InputArrayOfArrays contours,
//                                  int contourIdx, const Scalar& color,
//                                  int thickness=1, int lineType=8,
//                                  InputArray hierarchy=noArray(),
//                                  int maxLevel=INT_MAX, Point offset=Point() );

    vector<vector<Point> > vvp;
    vvp.push_back(hand.contour);
    drawContours(image, vvp, 0, CV_RGB(0,0,255), 1, CV_AA);

    //! draws the circle outline or a solid circle in the image
    circle(image, hand.center, 5, Scalar(255, 0, 255), 1, CV_AA);
    circle(image, hand.center, hand.radius, Scalar(255, 0, 0), 1, CV_AA);

    for (int i = 0; i < hand.fingers.size(); i++) {
        Point p = hand.fingers.at(i);
        circle(image, p, 10, Scalar(0, 255, 0), 3, CV_AA, 0);
        line(image, hand.center, p, Scalar(255,255,0), 1, CV_AA, 0);
    }

    for (int i = 0; i < hand.defects.size(); i++) {
        circle(image, hand.defects[i], 2, Scalar(200, 200, 200), 2, CV_AA, 0);
    }
//    }
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
