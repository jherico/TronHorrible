#include "ds.hpp"
#include <vector>

void yuy2rgb(unsigned char *dst, const unsigned char *src, const int width, const int height);

namespace Encom13 {
namespace Input {
namespace Softkinetic {

using namespace std;
using namespace boost;
using namespace DepthSense;

shared_ptr<Camera>  INSTANCE;

void Camera::worker() {
    Camera & camera = *INSTANCE.get();
    usleep(1000 * 400);
    camera.context.startNodes();
    camera.context.run();
    usleep(1000 * 400);
}

Camera::Camera() : context(Context::createStandalone()), colorDirty(false),  depthDirty(false) {
    INSTANCE = shared_ptr<Camera>(this);
    vector<Device> da = context.getDevices();
    if (da.size() >= 1) {
        vector<Node> na = da[0].getNodes();
        for (int n = 0; n < (int) na.size(); n++) {
            configureNode(na[n]);
        }
    }
    thread = shared_ptr<boost::thread>(new boost::thread(boost::bind(&Camera::worker)));
}

Camera::~Camera() {
    context.quit();
    context.stopNodes();
    if (cnode.isSet())
        context.unregisterNode(cnode);
    if (dnode.isSet())
        context.unregisterNode(dnode);
    if (anode.isSet())
        context.unregisterNode(anode);
}

void Camera::onNewColorSample(ColorNode node, ColorNode::NewSampleReceivedData data) {
    int32_t w, h;
    FrameFormat_toResolution(data.captureConfiguration.frameFormat, &w, &h);

    mutex::scoped_lock lock(mutex);
    if (colorImage.rows != h || colorImage.cols != w) {
        colorImage.create(h, w, CV_8UC3);
    }
    yuy2rgb(colorImage.data, data.colorMap, w, h);
    colorDirty = true;
}

void Camera::onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data) {
    int32_t w, h;
    FrameFormat_toResolution(data.captureConfiguration.frameFormat, &w, &h);

    mutex::scoped_lock lock(mutex);
    if (depthImage.rows != h || depthImage.cols != w) {
        depthImage.create(h, w, CV_32F);
    }
    memcpy(depthImage.data, data.depthMapFloatingPoint, h * w * 4);
    float * pdest = (float *) depthImage.data;
    for (int y = 0; y < h; ++y) {
        int rowoffset = y * w;
        for (int x = 0; x < w; ++x) {
            int offset = rowoffset + x;
            float & dest = *(pdest + offset);
            if (dest < 0) {
                dest = 0;
            }
        }
    }
    depthDirty = true;
}


cv::Mat Camera::getDepthImage() {
    mutex::scoped_lock lock(mutex);
    depthDirty = false;
    return depthImage.clone();
}

cv::Mat Camera::getColorImage() {
    mutex::scoped_lock lock(mutex);
    colorDirty = false;
    return colorImage.clone();
}

void colorCallback(ColorNode node, ColorNode::NewSampleReceivedData data) {
    INSTANCE.get()->onNewColorSample(node, data);
}

void depthCallback(DepthNode node, DepthNode::NewSampleReceivedData data) {
    INSTANCE.get()->onNewDepthSample(node, data);
}

void Camera::configureNode(Node & node) {
    if ((node.is<DepthNode>()) && (!dnode.isSet())) {
        dnode = node.as<DepthNode>();
        dnode.newSampleReceivedEvent().connect(&depthCallback);
        DepthNode::Configuration config = dnode.getConfiguration();
        config.frameFormat = FRAME_FORMAT_QVGA;
        config.framerate = 25;
        config.mode = DepthNode::CAMERA_MODE_CLOSE_MODE;
        config.saturation = false;
        dnode.setEnableDepthMapFloatingPoint(true);
        context.requestControl(dnode, 0);
        dnode.setConfiguration(config);
        context.registerNode(node);
    }
//
//    if ((node.is<ColorNode>()) && (!cnode.isSet())) {
//        cnode = node.as<ColorNode>();
//        cnode.newSampleReceivedEvent().connect(&colorCallback);
//
//        ColorNode::Configuration config = cnode.getConfiguration();
//        config.frameFormat = FRAME_FORMAT_QVGA;
//        config.compression = COMPRESSION_TYPE_YUY2;
//        config.powerLineFrequency = POWER_LINE_FREQUENCY_50HZ;
//        config.framerate = 25;
//        cnode.setEnableColorMap(true);
//        context.requestControl(cnode, 0);
//        cnode.setConfiguration(config);
//        context.registerNode(node);
//    }
//
//   if ((node.is<AudioNode>()) && (!anode.isSet())) {
//        anode = node.as<AudioNode>();
//        {
//            AudioNode::Configuration config = anode.getConfiguration();
//            context.requestControl(anode, 0);
//            config.sampleRate = 44100;
//            anode.setConfiguration(config);
//            anode.setInputMixerLevel(0.5f);
//        }
//        context.registerNode(node);
//    }
}

}
}
}
//
//int th_min = 650;
//int th_max = 2860;

// From SoftKinetic
// convert a YUY2 image to RGB

void yuy2rgb(unsigned char *dst, const unsigned char *src, const int width, const int height) {
  int x, y;
  const int width2 = width * 2;
  const int width4 = width * 3;
  const unsigned char *src1 = src;
  unsigned char *dst1 = dst;

  for (y=0; y<height; y++) {
    for (x=0; x<width; x+=2) {
      int x2=x*2;
      int y1  = src1[x2  ];
      int y2  = src1[x2+2];
      int u   = src1[x2+1] - 128;
      int v   = src1[x2+3] - 128;
      int uvr = (          15748 * v) / 10000;
      int uvg = (-1873 * u - 4681 * v) / 10000;
      int uvb = (18556 * u          ) / 10000;

      int x4=x*3;
      int r1 = y1 + uvr;
      int r2 = y2 + uvr;
      int g1 = y1 + uvg;
      int g2 = y2 + uvg;
      int b1 = y1 + uvb;
      int b2 = y2 + uvb;

      dst1[x4+0] = (b1 > 255) ? 255 : ((b1 < 0) ? 0 : b1);
      dst1[x4+1] = (g1 > 255) ? 255 : ((g1 < 0) ? 0 : g1);
      dst1[x4+2] = (r1 > 255) ? 255 : ((r1 < 0) ? 0 : r1);
      //dst1[x4+3] = 255;

      dst1[x4+3] = (b2 > 255) ? 255 : ((b2 < 0) ? 0 : b2);
      dst1[x4+4] = (g2 > 255) ? 255 : ((g2 < 0) ? 0 : g2);
      dst1[x4+5] = (r2 > 255) ? 255 : ((r2 < 0) ? 0 : r2);
    }
    src1 += width2;
    dst1 += width4;
  }
}

//void yuy2rgb(unsigned char *dst, const unsigned char *src, const int width, const int height) {
//    const int srcrow = width * 2;
//    const unsigned char * srcend = src + (height * srcrow);
//    while (src < srcend) {
//        int y1 = *src++;
//        int u = *src++ - 128;
//        int y2 = *src++;
//        int v = *src++ - 128;
//        int uvr = (15748 * v) / 10000;
//        int uvg = (-1873 * u - 4681 * v) / 10000;
//        int uvb = (18556 * u) / 10000;
//        *dst++ = std::max(0, std::min(255, y1 + uvr));
//        *dst++ = std::max(0, std::min(255, y1 + uvg));
//        *dst++ = std::max(0, std::min(255, y1 + uvb));
//        *dst++ = std::max(0, std::min(255, y2 + uvr));
//        *dst++ = std::max(0, std::min(255, y2 + uvg));
//        *dst++ = std::max(0, std::min(255, y2 + uvb));
//    }
//}


