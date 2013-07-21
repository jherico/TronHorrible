#include "RiftApp.hpp"

#include <osg/Config>
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

using namespace std;
using namespace osg;
using namespace boost;
using namespace Encom13;
using namespace Encom13::GL;

class RiftOsgApp : public RiftApp {
public:
    osgViewer::Viewer osgViewer;
    osg::observer_ptr<osgViewer::GraphicsWindow> osgWindow;

    RiftOsgApp(int x = 0, int y = 0, int width = 480, int height = 300) : RiftApp(x, y, width, height) {
    }

    osg::Geode* createScene() {
        osg::Program* program = new osg::Program();
        osg::Shader*  vert    = osg::Shader::readShaderFile(osg::Shader::VERTEX, "shaders/vertex.glsl");
        osg::Shader*  frag    = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, "shaders/fragment.glsl");
        program->addShader(vert);
        program->addShader(frag);
        osg::Geode*   geode   = new osg::Geode();
        geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 3.0)));
        geode->getOrCreateStateSet()->setAttribute(program);

        return geode;
    }

    void init() {
        RiftApp::init();

        osgWindow = osgViewer.setUpViewerAsEmbeddedInWindow(0,0,window.width / 2,window.height);
        osgViewer.setSceneData(createScene());
        osgViewer.addEventHandler(new osgViewer::StatsHandler);
        osgViewer.setCameraManipulator(new osgGA::TrackballManipulator());
        osgViewer.realize();
    }

    void renderScene(Eye eye) {
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        glViewport(0, 0, window.width / 2, window.height);
        osgViewer.getCamera()->setClearColor(osg::Vec4(
                eye == LEFT ? 1.0f : 0.5f, 0.5f, eye == LEFT ? 0.0f : 1.5f, 1.0f));
        osgViewer.frame();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void update(float time) {
    }

    void resize(int w, int h) {
        RiftApp::resize(w, h);
        // update the window dimensions, in case the window has been resized.
        if (osgWindow.valid())
        {
            osgWindow->resized(osgWindow->getTraits()->x, osgWindow->getTraits()->y, w / 2, h);
            osgWindow->getEventQueue()->windowResize(osgWindow->getTraits()->x, osgWindow->getTraits()->y, w / 2, h );
        }
    }

    void mousebutton( int button, int state, int x, int y )
    {
        if (osgWindow.valid())
        {
            if (state==0) osgWindow->getEventQueue()->mouseButtonPress( x, y, button+1 );
            else osgWindow->getEventQueue()->mouseButtonRelease( x, y, button+1 );
        }
    }

    void mousemove( int x, int y )
    {
        if (osgWindow.valid())
        {
            osgWindow->getEventQueue()->mouseMotion( x, y );
        }
    }

    bool keys( unsigned char key, int x, int y )
    {
        if (!App::keys(key, x, y)) {
            return false;
        }

        switch( key )
        {
            default:
                if (osgWindow.valid())
                {
                    osgWindow->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) key );
                    osgWindow->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) key );
                }
                break;
        }
        return true;
    }

};

int app_rift_osg_main(int argc, char **argv) {
    (new RiftOsgApp(100, -400))->run();
    return 0;
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

//      void FinishScene1()
//      {
//          float r, g, b, a;
//              glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        Program & distort = *distortProgram.get();
//        distort.use();
//        renderTexture(frameBuffer.getTexture());
//        const DistortionConfig & Distortion = this->sconfig.GetDistortionConfig();
//
//          float w = 1,
//                h = 1,
//                x = 0,
//                y = 0;
//
//          float as = float(16) / float(10);
//
//          // We are using 1/4 of DistortionCenter offset value here, since it is
//          // relative to [-1,1] range that gets mapped to [0, 0.5].
//          distort.setUniform2f("LensCenter",
//                                           x + (w + Distortion.XCenterOffset * 0.5f)*0.5f, y + h*0.5f);
//          distort.setUniform2f("ScreenCenter", x + w*0.5f, y + h*0.5f);
//
//          // MA: This is more correct but we would need higher-res texture vertically; we should adopt this
//          // once we have asymmetric input texture scale.
//          float scaleFactor = 1.0f / Distortion.Scale;
//
//          distort.setUniform2f("Scale",   (w/2) * scaleFactor, (h/2) * scaleFactor * as);
//          distort.setUniform2f("ScaleIn", (2/w),               (2/h) / as);
//
//          distort.setUniform4f("HmdWarpParam",
//                                           Distortion.K[0], Distortion.K[1], Distortion.K[2], Distortion.K[3]);
//
//          Matrix4f texm(w, 0, 0, x,
//                        0, h, 0, y,
//                        0, 0, 0, 0,
//                        0, 0, 0, 1);
//          distort.setUniform4x4f("Texm", &texm.M[0][0]);
//
//          Matrix4f view(2, 0, 0, -1,
//                        0, 2, 0, -1,
//                         0, 0, 0, 0,
//                         0, 0, 0, 1);
//
////        ShaderFill fill(pPostProcessShader);
////        fill.SetTexture(0, pSceneColorTex);
////        RenderWithAlpha(&fill, pFullScreenVertexBuffer, NULL, view, 0, 4, Prim_TriangleStrip);
//              renderTexture(frameBuffer.getTexture());
//
//      }
