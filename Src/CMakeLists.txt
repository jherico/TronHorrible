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

set(SOURCE_FILES 
    App.cpp
    EGL.cpp
    Geometry.cpp
    Program.cpp
    Common.cpp
)

set(EXTRA_LIBS 
    boost_system boost_filesystem opencv_video opencv_core opencv_highgui opencv_imgproc
)

if (RPI)
    list(APPEND SOURCE_FILES 
        EGL_RPI.cpp
    )
    include_directories(/opt/vc/include)
    include_directories(/opt/vc/include/interface/vcos/pthreads)
else()
    list(APPEND SOURCE_FILES 
        EGL_X11.cpp
    )
    
    list(APPEND EXTRA_LIBS
        GLESv2 EGL X11
    )
endif()


#set (SOURCE_DIR ${CMAKE_SOURCE_DIR}/../Src)

add_executable(TronHorrible ${SOURCE_FILES})

target_link_libraries(TronHorrible ${EXTRA_LIBS})
