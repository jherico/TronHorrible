#include <opencv2/opencv.hpp>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;


void bindNextFrame(GLuint texture) {
    static VideoCapture videoCapture(0);
    static bool init = false;
    if (!init) {
        cerr << "Setting up video" << endl;
        videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
        videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
        init = true;
    }

    cv::waitKey(1);
    if (!videoCapture.grab()) {
        cerr << "Failed grab" << endl;
        return;
        //throw string("failed to grab image");
    }
    static Mat frame;
    videoCapture.retrieve(frame);
//        cv::flip(frame, frame, 0);
//        cvtColor(frame, frame, CV_BGR2RGB);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
}
