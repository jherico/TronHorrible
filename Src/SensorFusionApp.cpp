#pragma once

#include "App.hpp"

namespace Encom13 {

class SensorFusionApp : public App {
public:
    SensorFusionApp(int x = 0, int y = 0, int width = 480, int height = 300);
    virtual ~SensorFusionApp() {
    }

    virtual void init() {

    }
    void display() {

    }
};

}

int sensor_fusion_main(int argc, char ** argv) {

}
