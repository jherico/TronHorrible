#include "common.hpp"

using namespace boost;
using namespace std;

int app_osg_main(int, char**);
int rift_video_main(int, char**);
int rift_boxes_main(int, char**);
int osg_animate_main(int, char**);
int app_rift_osg_main(int, char**);
int opencv_undistort_main(int, char**);
int midi_main(int, char**);
int egl_test_main(int, char**);
int test_nsb_main(int, char**);
int sensor_fusion_main(int, char**);
int camera_calibration_main(int, char**);
int undistort_gl_app(int, char**);
int gl_scratch_main(int, char**);

int main(int argc , char** argv) {
    int result = -1;
    cout << "Starting" << endl;
//    try {
        result = rift_video_main(argc, argv);
        //result = app_rift_main(argc, argv);
//    } catch (boost::exception & err) {
//        string * pfile = boost::get_error_info<errinfo_file_name>(err);
//        cout << "Got file " << *pfile << endl;
//    } catch (std::exception & err) {
//        cout << "Std exception caught" << endl;
//    } catch (...) {
//        cout << "Unknown exception caught" << endl;
//    }
    cout << "done" << endl;
    return result;
}
