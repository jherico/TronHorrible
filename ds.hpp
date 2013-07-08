
#pragma once

#include <opencv2/opencv.hpp>
#include <DepthSense.hxx>
#include <exception>
#include <vector>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace Encom13 {
namespace Input {
namespace Softkinetic {

class Camera {
    typedef DepthSense::DepthNode DNode;
    typedef DepthSense::ColorNode CNode;
    typedef DepthSense::AudioNode ANode;
    DepthSense::Context context;
    cv::Mat colorImage, depthImage;
    volatile bool colorDirty, depthDirty;
    DNode dnode;
    CNode cnode;
    ANode anode;
    boost::mutex mutex;
    boost::shared_ptr<boost::thread>  thread;

public:
    Camera();
    virtual ~Camera();
    cv::Mat getDepthImage();
    cv::Mat getColorImage();
    bool isDepthDirty() { return depthDirty; }
    bool isColorDirty() { return colorDirty; }

private:
    friend void colorCallback(CNode node, CNode::NewSampleReceivedData data);

    friend void depthCallback(DNode node, DNode::NewSampleReceivedData data);


    void onNewColorSample(CNode node, CNode::NewSampleReceivedData data);
    void onNewDepthSample(DNode node, DNode::NewSampleReceivedData data);
    void onNewAudioSample(ANode node, ANode::NewSampleReceivedData data);
    void configureNode(DepthSense::Node & node);
    static void worker();
    static void dsColorToCvMat(const CNode::NewSampleReceivedData & data, cv::Mat & output);
    static void dsDepthToCvMat(const DNode::NewSampleReceivedData & data, cv::Mat & output);
};

}
}
}
