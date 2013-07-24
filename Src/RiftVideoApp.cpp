#include "RiftApp.hpp"

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace cv;
using namespace boost;
using namespace Encom13;
using namespace Encom13::GL;
using namespace Encom13::GL::Shaders;
using namespace Encom13::Stereo;
using namespace Encom13::Blas;

//#define STEREO_FILE 1
//#define VIDEO_FILE "/home/bradd/Videos/drh.mkv"
//#define VIDEO_FILE "/home/bradd/Videos/monsters.mp4"
//#define VIDEO_FILE "/mnt/internal/Tron Legacy (2010).avi"
//#define VIDEO_FILE "/home/bdavis/Videos/LG.mkv"
//#define VIDEO_FILE "/home/bdavis/Videos/TRON.mkv"
//#define VIDEO_FILE "/home/bdavis/Videos/archer.avi"
//#define VIDEO_FILE "/mnt/media/Videos/3D/AVATAR 3D.mkv"
//#define VIDEO_FILE "/mnt/media/Videos/3D/Dredd.2012.1080p.BluRay.3D.HSBS.x264.YIFY.mp4"
//#define VIDEO_FILE "/mnt/media/Videos/3D/prometheus.2012.1080p.bluray.3D.h-sbs.dts.x264-publichd.mkv"
//#define VIDEO_FILE "/mnt/media/Videos/3D/I Robot 2004 1080p 3D HOU BDRip x264 ac3 vice.mkv"

#define PH 0.5
#define PQ 0.25
#define NH -0.5
#define NO -1
#define ON 1
#define ZE 0

class RiftVideoApp : public RiftApp {
public:
    Program & standardProgram;
    VideoCapture capture;
    GLuint textures[2];
    vector<GLuint> buffers;
    Vertex vertices[4];
    GLubyte indices[6];

//    xoffset float   0.26000005
//    yoffset float   0.049999997
//    aspect  float   1.59999943
//    int xoffset = 268, yoffset = 20;
    float xoffset, yoffset;
    float aspect;
//    int xoffset = 0, yoffset = 0;

    RiftVideoApp(int x = 0, int y = 0, int width = 480, int height = 300)
            : RiftApp(x, y, width, height), standardProgram(*Program::getProgram("textured").get()), vertices({
                Vertex::simpleTextured(glm::vec3(NO, NO, ZE), glm::vec2(ZE, ZE)), //
                Vertex::simpleTextured(glm::vec3(ON, NO, ZE), glm::vec2(ON, ZE)), //
                Vertex::simpleTextured(glm::vec3(ON, ON, ZE), glm::vec2(ON, ON)), //
                Vertex::simpleTextured(glm::vec3(NO, ON, ZE), glm::vec2(ZE, ON)) //
        }), indices({ 0, 1, 2, 2, 3, 0, }) {
        xoffset = 0.26;
        yoffset = 0.05;
        aspect = 1.6;
    }

    void init() {
        RiftApp::init();
        // renders to a single eye per call
        glGenTextures(2, textures);
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

#ifdef VIDEO_FILE
        capture = VideoCapture(VIDEO_FILE);
#else
        capture = VideoCapture(0);
        capture.set( CV_CAP_PROP_FRAME_WIDTH, 1280);
        capture.set( CV_CAP_PROP_FRAME_HEIGHT,720);

#endif
        buffer.init(1280, 1600);
        buffers.resize(2);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.5, 1.0);
        glGenBuffers(2, &buffers[0]);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


    void renderScene(Eye eye) {
        glClearColor(0.0, 0.0, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        Program::getProgram("colored").get()->use();
        Program::setActiveProjection(glm::perspective<float>(110.0, 0.8 * 2, 0.01, 1000.0));
        Program::setActiveModelview(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
        Utils::renderWireColorCube();

        Program::setActiveModelview(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)));
        Utils::renderWireColorCube();

        Program::setActiveModelview(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)));
        Utils::renderWireColorCube();

//        standardProgram.use();
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, textures[0]);
//        GLint samplerLoc = Program::getActiveProgram().getLocation(UNIFORM, "s_texture");
//        glUniform1i(samplerLoc, 0);
//
//        Vertex myVertices[] = {
//            Vertex(vertices[0]), //
//            Vertex(vertices[1]), //
//            Vertex(vertices[2]), //
//            Vertex(vertices[3]), //
//        };
//        for (int i = 0; i < 4; ++i) {
//            if (aspect < 1) {
//                myVertices[i].pos.x *= aspect;
//            } else {
//                myVertices[i].pos.y /= aspect;
//            }
//            if (0 == eye) {
//                myVertices[i].pos.x += xoffset;
//                myVertices[i].pos.y += yoffset;
//            } else {
//                myVertices[i].pos.x -= xoffset;
//                myVertices[i].pos.y -= yoffset;
//            }
//        }
//
//        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, myVertices, GL_DYNAMIC_DRAW);
//
//        GLint ploc = standardProgram.getPositionAttrib();
//        GLint tloc = standardProgram.getTextureAttrib();
//        glEnableVertexAttribArray(ploc);
//        glVertexAttribPointer(ploc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, pos));
//        glEnableVertexAttribArray(tloc);
//        glVertexAttribPointer(tloc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tex));
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*) 0);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//        Utils::renderFullscreenTexture(texture);
    }

    void update(float time) {
        if (!capture.grab()) {
            throw std::exception();
        }

        Mat frame(1280, 720, CV_32S);
        capture.retrieve(frame);
        cv::flip(frame, frame, 0);
        cvtColor(frame, frame, CV_BGR2RGB);
//        int crop = 100 * 3;
//        frame = frame(Rect(crop, 0, frame.cols - crop * 2, frame.rows)).clone();
//        for (int i = 0; i < 2; ++i) {
#ifdef STEREO_FILE
        static int w = frame.cols;
        static int h = frame.rows;
        for (int i = 0; i < 2; ++i) {
            static Rect rects[] = { Rect(0, 0, w / 2, h), Rect(w / 2, 0, w / 2, h) };
            const Rect & rect = rects[i];
            Mat newImage;
            Mat(frame, rect).copyTo(newImage);
            bindImage(newImage, textures[i]);
        }
#else
//        if (xoffset != 0 || yoffset != 0) {
//            int localx = xoffset;
//            int localy = yoffset;
//            if (i == 1) {
//                localx = -localx;
//                localy = -localy;
//            }
//                    // This approach keeps the whole image, but increasingly distorts it
//                    Size newSize(frame.cols + abs(xoffset), frame.rows + abs(yoffset));
//                    Mat eyeTexture = Mat::zeros(newSize, frame.type());//  = frame.clone();
//                    frame.copyTo(eyeTexture(Rect(localx > 0 ? localx: 0, localy > 0 ? localy: 0,
//                            frame.cols, frame.rows)));
//                    bindImage(eyeTexture, textures[i]);
//                    // This approach doesn't distory but eliminates increasing portions of the image
//            Mat eyeTexture = Mat::zeros(frame.rows, frame.cols, frame.type());//  = frame.clone();
//            Size subImageSize(frame.cols - abs(localx), frame.rows - abs(localy));
//            Point subImageStart(localx > 0 ? localx: 0, localy > 0 ? localy: 0);
//            Mat subImage = frame(Rect(subImageStart, subImageSize));
//            subImage.copyTo(eyeTexture(Rect(Point(0, 0), subImageSize)));
//            bindImage(eyeTexture, textures[i]);
//        } else {
        for (int i = 0; i < 2; ++i) {
            bindImage(frame, textures[i]);
        }
//        }
#endif
    }

    static void bindImage(Mat image, int texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    }


#define INCR 0.01

    bool keys(unsigned char key, int x, int y) {
        switch (key) {
            case 'f':
                aspect += INCR;
                break;
            case 'g':
                aspect -= INCR;
                break;
            case 'w':
                yoffset += INCR;
                break;
            case 's':
                yoffset -= INCR;
                break;
            case 'a':
                xoffset += INCR;
                break;
            case 'd':
                xoffset -= INCR;
                break;

        }
        return RiftApp::keys(key, x, y);
    }

};

int app_rift_main(int argc, char **argv) {
//    (new RiftVideoApp(1920, 1080 * 2 - 800, 1280, 800))->run();
    (new RiftVideoApp(1920, 0, 1280, 800))->run();
//    (new RiftVideoApp(0, 0, 1280, 800))->run();
    return 0;
}
