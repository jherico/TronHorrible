#include "common.hpp"

#include <OVR.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>

typedef std::vector<unsigned char> ewkb_t;
using namespace std;

//
//int test_nsb_main(int argc, char ** argv) {
//    OVR_HANDLE handle = ovrOpenRiftRecording("../OculusSDK/Samples/data/rift1.json");
//    sleep(1);
//    ovrRegisterSampleHandler(handle, callback);
//    sleep(1000);
//
//    return 0;
//}
