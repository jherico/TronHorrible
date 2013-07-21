#include <math.h>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <alsa/asoundlib.h>
#include <algorithm>
#include "EGL.hpp"
#include "common.hpp"
//#include "ds.hpp"
//#include "hand.hpp"

//using namespace std;
//using namespace cv;
////using namespace Encom13::Input::Softkinetic;
//
//#define VIDEO_FILE "/mnt/internal/Tron Legacy (2010).avi"
//
//#define NUM_FINGERS 5
//#define NUM_DEFECTS 8
//
//GLfloat vertices[] = { -1, -1, 0, 1, 0, 1, 0, 1,
//                        1, -1, 0, 1, 1, 1, 0, 1,
//                        1,  1, 0, 1, 1, 0, 0, 1,
//                       -1,  1, 0, 1, 0, 0, 0, 1, };
//
//GLubyte indices[] = { 0, 1, 2, 2, 3, 0, };
//
//class App {
//public:
//    typedef boost::shared_ptr<GL::ES::Program> ProgramPtr;
//    GLuint texture;
//    GLuint vertexBuffer;
//    GLuint indexBuffer;
//    ESContext esContext;
//    ProgramPtr program;
//    GLint vloc;
//    GLint tloc;
//
//    VideoCapture capture;
//    Mat frame;
//    Mat thr_image;
//    vector<Point> contour;
//    bool contourFound;
//    Point hand_center;
//
//    Mat kernel; /* Kernel for morph operations */
//
//    int num_fingers;
//    int hand_radius;
//    int num_defects;
//
//    App() {
//        esInitContext(&esContext);
//        esContext.userData = this;
//
//        initUi();
////        initGl();
//        initCv();
//    }
//
//    void run() {
//        while ('q' != cvWaitKey(1)) {
//                update(1);
//                usleep(1000);
//        }
//
////        esMainLoop(&esContext);
//    }
//
//    void initCv() {
//        //capture = cvCreateFileCapture(VIDEO_FILE);
//        capture = VideoCapture(0);
//        capture.retrieve(frame);
//
//        //IplImage *image = cvLoadImage("/usr/share/backgrounds/gnome/FootFall.png");
//        //texture = ConvertIplToTexture(image);
//        //cvReleaseImage(&image);
//    }
//
//    void initUi() {
//        cvNamedWindow("output", CV_WINDOW_AUTOSIZE);
//        cvNamedWindow("thresholded", CV_WINDOW_AUTOSIZE);
//        cvMoveWindow("output", 50, 50);
//        cvMoveWindow("thresholded", 700, 50);
//
////        esCreateWindow(&esContext, "Hello Triangle", 320, 240, ES_WINDOW_RGB, 100, -700);
////        esRegisterDrawFunc(&esContext, ::display);
////        esRegisterUpdateFunc(&esContext, ::update);
////        esRegisterKeyFunc(&esContext, ::keys);
//
//    }
//
//    void initGl() {
//        glViewport(0, 0, (GLsizei) esContext.width, (GLsizei) esContext.height);
//
//        program = ProgramPtr(new GL::ES::Program("shaders/textured.vs", "shaders/textured.fs"));
//        vloc = glGetAttribLocation(program.get()->program, "a_position");
//        tloc = glGetAttribLocation(program.get()->program, "a_texCoord");
//
//        glGenTextures(1, &texture);
//        glBindTexture(GL_TEXTURE_2D, texture);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glBindTexture(GL_TEXTURE_2D, 0);
//
//        glGenBuffers(1, &vertexBuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//        int size = sizeof(vertices);
//        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
//        glGenBuffers(1, &indexBuffer);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//        size = sizeof(indices);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_DYNAMIC_DRAW);
//        glEnable(GL_DEPTH_TEST);
//        glClearColor(0.0, 0.0, 0.5, 1.0);
//    }
//
//    void display() {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // Use the program object
//        glUseProgram(program.get()->program);
//
//        // Bind the texture
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture);
//
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
//
//        glEnableVertexAttribArray(vloc);
//        glEnableVertexAttribArray(tloc);
//
//        // Load the vertex position
//        int stride = sizeof(GLfloat) * 8;
//        glVertexAttribPointer(vloc, 4, GL_FLOAT, GL_FALSE, stride, 0);
//
//        // Load the texture coordinate
//        glVertexAttribPointer(tloc, 4, GL_FLOAT, GL_FALSE, stride, (void*) (sizeof(GLfloat) * 4));
//
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*) 0);
//
//    }
//
//    void update(float time) {
//        capture.grab();
//        bool b = capture.retrieve(frame);
//        imshow("output", frame);
//
//        Encom13::Gesture::Hand hand;
//        hand.filter_and_threshold(frame, thr_image);
//        hand.find_contour(thr_image);
////        hand.find_convex_hull();
////        find_fingers();
//        hand.annotate(frame);
//
////        cvShowImage("thresholded", thr_image);
//        //usleep(100000/24);
//        imshow("output", frame);
////        imshow("thresholded", thr_image);
//
//        cvtColor(frame, frame, CV_BGR2RGB);
//        bindImage(frame);
//    }
//
//    void bindImage(Mat image) {
//        glBindTexture(GL_TEXTURE_2D, texture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//    }
//
//    void keys(unsigned char key, int x, int y) {
//        switch (key) {
//        case 27:
//        case 'q':
//            exit(0);
//            break;
//        }
//    }
//
//};
//
//
//void display(ESContext* context) {
//    ((App*) context->userData)->display();
//}
//
//void update(ESContext* context, float time) {
//    ((App*) context->userData)->update(time);
//}
//
//void keys(ESContext* context, unsigned char a, int b, int c) {
//    ((App*) context->userData)->keys(a, b, c);
//}
//
//int main2(int argc, char **argv) {
//    cout << "Starting" << endl;
//    try {
//        (new App())->run();
//    } catch (string * err) {
//        const char * errStr = err->c_str();
//        cerr << errStr << endl;
//
//    }
//    cout << "done" << endl;
//    return 0;
//}
//
//
//void midi_action2(snd_seq_t *seq_handle) {
//
//    snd_seq_event_t *ev;
//
//  do {
//    snd_seq_event_input(seq_handle, &ev);
//    snd_seq_ev_ctrl_t & control = ev->data.control;
//    switch (ev->type) {
//      case SND_SEQ_EVENT_CONTROLLER:
//          fprintf(stderr, "Control event on Channel %2d: %5d %5d %5d \r",
//                  ev->data.control.channel, ev->data.control.value, control.param, (int)control.channel);
//        break;
//      case SND_SEQ_EVENT_PITCHBEND:
//        fprintf(stderr, "Pitchbender event on Channel %2d: %5d %5d %5d \r",
//                ev->data.control.channel, ev->data.control.value, control.param, (int)control.channel);
//        break;
//      case SND_SEQ_EVENT_NOTEON:
//        fprintf(stderr, "Note On event on Channel %2d: %5d       \r",
//                ev->data.control.channel, ev->data.note.note);
//        break;
//      case SND_SEQ_EVENT_NOTEOFF:
//        fprintf(stderr, "Note Off event on Channel %2d: %5d      \r",
//                ev->data.control.channel, ev->data.note.note);
//        break;
//    }
//    snd_seq_free_event(ev);
//  } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
//}
//
//void processMidi() {
//    snd_seq_t * handle;
//    int err = snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0);
//    if (err < 0)
//        exit(1);
//    snd_seq_set_client_name(handle, "OpenCV");
//    int my_port = snd_seq_create_simple_port(handle, "port 0",
//            SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
//             SND_SEQ_PORT_TYPE_APPLICATION);
//
//    int my_output_port = snd_seq_create_simple_port(handle, "port 1",
//            SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
//            SND_SEQ_PORT_TYPE_APPLICATION);
//
//    if (my_port < 0) {
//        exit(1);
//    }
//    int npfd = snd_seq_poll_descriptors_count(handle, POLLIN);
//    pollfd* pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
//    snd_seq_poll_descriptors(handle, pfd, npfd, POLLIN);
//    while (1) {
//      if (poll(pfd, npfd, 100000) > 0) {
//          midi_action2(handle);
//      }
//      usleep(1000);
//
////      snd_seq_event_t ev;
////      snd_seq_ev_clear(&ev);
////      snd_seq_ev_set_source(&ev, my_output_port);
////      snd_seq_ev_set_subs(&ev);
////      snd_seq_ev_set_direct(&ev);
////      snd_seq_event_output(seq, &ev);
////      snd_seq_drain_output(seq);
//     }
//    }
//    //    snd_seq_addr_t addr; memset(&addr, 0, sizeof(addr));
//    //    snd_seq_parse_address(handle, &addr, "FLUID");
//    //    snd_seq_parse_address(handle, &addr, "QmidiNet:0");
//    //    int result = snd_seq_connect_from(handle, my_port, 128, 0);
//}
//
///*----------------------------------------------------------------------------*/
//int main(int argc, char* argv[]) {
//    boost::thread thread(boost::bind(processMidi));
//    namedWindow("color", CV_WINDOW_AUTOSIZE);
//    namedWindow("depth", CV_WINDOW_AUTOSIZE);
//    namedWindow("fizz", CV_WINDOW_AUTOSIZE);
//    namedWindow("bizz", CV_WINDOW_AUTOSIZE);
//    int erosion_size = 2;
//    /// Create Erosion Trackbar
////    createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo", &erosion_size, 100, &callback);
//    moveWindow("color", 50, 50);
//    moveWindow("depth", 400, 50);
//    moveWindow("fizz", 50, 400);
//    moveWindow("bizz", 400, 400);
//
//    /// Show some stuff
////    callback(1, 0);
//    waitKey(1);
//    Encom13::Input::Softkinetic::Camera camera;
//    usleep(1000 * 1000);
//
//    bool quit = false;
//    while (!quit) {
//        if (camera.isColorDirty()) {
//            imshow("color", camera.getColorImage());
//        }
//        if (camera.isDepthDirty()) {
//            Mat depthOriginal = camera.getDepthImage();
//            Mat mask1;
//            Encom13::Gesture::Hand hand;
//            hand.filter_and_threshold_ds(depthOriginal, mask1);
//            cvtColor(depthOriginal, depthOriginal, CV_GRAY2RGB);
//            // Create a standard color image to annotate with hand findings
////            Mat color;
////            {
////                double min, max;
////                cv::minMaxIdx(depthOriginal, &min, &max);
////                cv::convertScaleAbs(depthOriginal, color, 255 / (max - min));
////                applyColorMap(color, color, cv::COLORMAP_BONE);
////            }
//
//            static vector<Point> contour;
//            if (hand.find_contour(mask1)) {
//                hand.find_convex_hull();
//                hand.find_fingers(mask1);
//                cvtColor(mask1, mask1, CV_GRAY2RGB);
//                hand.annotate(mask1);
//            }
////            imshow("depth", depthMasked);
////            imshow("color", color);
//            imshow("fizz", mask1);
//            imshow("bizz", depthOriginal);
//
//            //Mat thr_image;
//            //inRange(g_depthImage, Scalar(th_min), Scalar(th_max), thr_image);
//            //imshow("color", threshold);
//        }
//
//        char key = cvWaitKey(1);
//        switch (key) {
//        case 27:
//        case 'q':
//            quit = true;
//            break;
//
//        default:
//            usleep(1000);
//            break;
//        }
//
//    }
//// Get the list of currently connected devices
//
//    return 0;
//}
//
////
////double min, max;
////cv::minMaxIdx(g_depthImage, &min, &max);
////cv::Mat adjMap;
////cv::convertScaleAbs(g_depthImage, adjMap, 255 / (max - min));
////Mat thr_image;
////inRange(g_depthImage, Scalar(th_min), Scalar(th_max), thr_image);
////
////cv::Mat falseColorsMap;
////
/////* OpenCV display - this will slow stuff down, should be in thread*/
////
////imshow("color", thr_image);
////imshow("depth", adjMap);
////
////// Allow OpenCV to shut down the programadjMap
//
////            g_context.run();
////            g_context.stopNodes();
