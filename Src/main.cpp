#include "common.hpp"

using namespace boost;
using namespace std;

int app_osg_main(int, char**);
int app_rift_main(int, char**);
int osg_animate_main(int, char**);
int app_rift_osg_main(int, char**);
int opencv_undistort_main(int, char**);
int midi_main(int, char**);
int egl_test_main(int, char**);


int main(int argc , char** argv) {
    int result = -1;
    cout << "Starting" << endl;
//    try {
        result = app_rift_main(argc, argv);
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
