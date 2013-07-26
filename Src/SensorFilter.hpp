/************************************************************************************

 PublicHeader:   OVR.h
 Filename    :   OVR_SensorFilter.h
 Content     :   Basic filtering of sensor data
 Created     :   March 7, 2013
 Authors     :   Steve LaValle, Anna Yershova

 Copyright   :   Copyright 2012 Oculus VR, Inc. All Rights reserved.

 Use of this software is subject to the terms of the Oculus license
 agreement provided at the time of installation or download, or which
 otherwise accompanies this software in either electronic or hard copy form.

 *************************************************************************************/

#pragma once
#include "common.hpp"

// This class maintains a sliding window of sensor data taken over time and implements
// various simple filters, most of which are linear functions of the data history.
class SensorFilter {
    enum {
        MaxFilterSize = 100,
        DefaultFilterSize = 20
    };

private:
    typedef boost::circular_buffer<glm::vec3> Buffer;
    typedef Buffer::iterator BufferItr;
    typedef Buffer::const_iterator BufferCItr;
    Buffer buffer;

public:
    // Create a new filter with size i
    SensorFilter(int i = DefaultFilterSize) {
        buffer.resize(i);
    }

    // Create a new element to the filter
    void AddElement(const glm::vec3 &e) {
        buffer.push_front(e);
    }

    // Get element i.  0 is the most recent, 1 is one step ago, 2 is two steps ago, ...
    glm::vec3 GetPrev(int i) const {
        return buffer[i];
    }

    // Simple statistics
    glm::vec3 Total() const;
    glm::vec3 Mean() const;
    glm::vec3 Median() const;
    glm::vec3 Variance() const; // The diagonal of covariance matrix
    glm::mat4 Covariance() const;
    glm::vec3 PearsonCoefficient() const;

    // A popular family of smoothing filters and smoothed derivatives
    glm::vec3 SavitzkyGolaySmooth8() const;
    glm::vec3 SavitzkyGolayDerivative4() const;
    glm::vec3 SavitzkyGolayDerivative5() const;
    glm::vec3 SavitzkyGolayDerivative12() const;
    glm::vec3 SavitzkyGolayDerivativeN(int n) const;

    ~SensorFilter() {
    }
};

