project(TronHorrible)

cmake_minimum_required(VERSION 2.8)

# Support our own custom modules
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/CMake/Modules
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_VERBOSE_MAKEFILE ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()


set(EXTRA_LIBS 
    GLESv2 EGL X11 boost_system  boost_filesystem opencv_video opencv_core opencv_highgui opencv_imgproc
)

#set (SOURCE_DIR ${CMAKE_SOURCE_DIR}/../Src)

add_executable(TronHorrible
    App.cpp
    EGL.cpp
    EGL_X11.cpp
    Geometry.cpp
    Program.cpp
    Common.cpp
)

target_link_libraries(TronHorrible ${EXTRA_LIBS})
