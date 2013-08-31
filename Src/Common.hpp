#pragma once

#include <string>
#include <fstream>
#include <exception>
#include <iostream>
#include <algorithm>

#include <cmath>
#include <cstring>
#include <cstdlib>

#include <boost/exception/all.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>

#include <opencv2/opencv.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>


#define GL_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

std::string slurp(const std::string& in);
std::string slurp(std::ifstream& in);



