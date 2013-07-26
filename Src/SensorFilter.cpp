/************************************************************************************

 PublicHeader:   OVR.h
 Filename    :   OVR_SensorFilter.cpp
 Content     :   Basic filtering of sensor data
 Created     :   March 7, 2013
 Authors     :   Steve LaValle, Anna Yershova

 Copyright   :   Copyright 2012 Oculus VR, Inc. All Rights reserved.

 Use of this software is subject to the terms of the Oculus license
 agreement provided at the time of installation or download, or which
 otherwise accompanies this software in either electronic or hard copy form.

 *************************************************************************************/

#include "SensorFilter.hpp"

glm::vec3 SensorFilter::Total() const {
    glm::vec3 total = glm::vec3();
    for (BufferCItr itr = buffer.begin(); itr != buffer.end(); ++itr) {
        total += *itr;
    }
    return total;
}

glm::vec3 SensorFilter::Mean() const {
    return Total()/(float)buffer.size();
}

glm::vec3 SensorFilter::Median() const {
    int half_window = (int) buffer.size() / 2;
    float sortx[MaxFilterSize];
    float resultx = 0.0f;

    float sorty[MaxFilterSize];
    float resulty = 0.0f;

    float sortz[MaxFilterSize];
    float resultz = 0.0f;

    for (size_t i = 0; i < buffer.size(); i++) {
        sortx[i] = buffer[i].x;
        sorty[i] = buffer[i].y;
        sortz[i] = buffer[i].z;
    }
    for (int j = 0; j <= half_window; j++) {
        int minx = j;
        int miny = j;
        int minz = j;
        for (size_t k = j + 1; k < buffer.size(); k++) {
            if (sortx[k] < sortx[minx])
                minx = k;
            if (sorty[k] < sorty[miny])
                miny = k;
            if (sortz[k] < sortz[minz])
                minz = k;
        }
        const float tempx = sortx[j];
        const float tempy = sorty[j];
        const float tempz = sortz[j];
        sortx[j] = sortx[minx];
        sortx[minx] = tempx;

        sorty[j] = sorty[miny];
        sorty[miny] = tempy;

        sortz[j] = sortz[minz];
        sortz[minz] = tempz;
    }
    resultx = sortx[half_window];
    resulty = sorty[half_window];
    resultz = sortz[half_window];

    return glm::vec3(resultx, resulty, resultz);
}

//  Only the diagonal of the covariance matrix.
glm::vec3 SensorFilter::Variance() const {
    glm::vec3 mean = Mean();
    glm::vec3 total = glm::vec3(0.0f, 0.0f, 0.0f);
    for (size_t i = 0; i < buffer.size(); i++) {
        total.x += (buffer[i].x - mean.x) * (buffer[i].x - mean.x);
        total.y += (buffer[i].y - mean.y) * (buffer[i].y - mean.y);
        total.z += (buffer[i].z - mean.z) * (buffer[i].z - mean.z);
    }
    return total / (float) buffer.size();
}
//
//// Should be a 3x3 matrix returned, but OVR_math.h doesn't have one
//glm::mat3 SensorFilter::Covariance() const {
//    glm::vec3 mean = Mean();
//    glm::mat3 total = glm::mat3(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
//    for (int i = 0; i < Size; i++) {
//        total.M[0][0] += (buffer[i].x - mean.x) * (buffer[i].x - mean.x);
//        total.M[1][0] += (buffer[i].y - mean.y) * (buffer[i].x - mean.x);
//        total.M[2][0] += (buffer[i].z - mean.z) * (buffer[i].x - mean.x);
//        total.M[1][1] += (buffer[i].y - mean.y) * (buffer[i].y - mean.y);
//        total.M[2][1] += (buffer[i].z - mean.z) * (buffer[i].y - mean.y);
//        total.M[2][2] += (buffer[i].z - mean.z) * (buffer[i].z - mean.z);
//    }
//    total.M[0][1] = total.M[1][0];
//    total.M[0][2] = total.M[2][0];
//    total.M[1][2] = total.M[2][1];
//    for (int i = 0; i < 3; i++)
//        for (int j = 0; j < 3; j++)
//            total.M[i][j] *= 1.0f / Size;
//    return total;
//}
//
//glm::vec3 SensorFilter::PearsonCoefficient() const {
//    glm::mat3 cov = Covariance();
//    glm::vec3 pearson = glm::vec3();
//    pearson.x = cov.M[0][1] / (sqrt(cov.M[0][0]) * sqrt(cov.M[1][1]));
//    pearson.y = cov.M[1][2] / (sqrt(cov.M[1][1]) * sqrt(cov.M[2][2]));
//    pearson.z = cov.M[2][0] / (sqrt(cov.M[2][2]) * sqrt(cov.M[0][0]));
//
//    return pearson;
//}

glm::vec3 SensorFilter::SavitzkyGolaySmooth8() const {
    return GetPrev(0) * 0.41667f + GetPrev(1) * 0.33333f + GetPrev(2) * 0.25f + GetPrev(3) * 0.16667f
            + GetPrev(4) * 0.08333f - GetPrev(6) * 0.08333f - GetPrev(7) * 0.16667f;
}

glm::vec3 SensorFilter::SavitzkyGolayDerivative4() const {
    return GetPrev(0) * 0.3f + GetPrev(1) * 0.1f - GetPrev(2) * 0.1f - GetPrev(3) * 0.3f;
}

glm::vec3 SensorFilter::SavitzkyGolayDerivative5() const {
    return GetPrev(0) * 0.2f + GetPrev(1) * 0.1f - GetPrev(3) * 0.1f - GetPrev(4) * 0.2f;
}

glm::vec3 SensorFilter::SavitzkyGolayDerivative12() const {
    return GetPrev(0) * 0.03846f + GetPrev(1) * 0.03147f + GetPrev(2) * 0.02448f + GetPrev(3) * 0.01748f
            + GetPrev(4) * 0.01049f + GetPrev(5) * 0.0035f - GetPrev(6) * 0.0035f - GetPrev(7) * 0.01049f
            - GetPrev(8) * 0.01748f - GetPrev(9) * 0.02448f - GetPrev(10) * 0.03147f - GetPrev(11) * 0.03846f;
}

glm::vec3 SensorFilter::SavitzkyGolayDerivativeN(int n) const {
    int m = (n - 1) / 2;
    glm::vec3 result = glm::vec3();
    for (int k = 1; k <= m; k++) {
        int ind1 = m - k;
        int ind2 = n - m + k - 1;
        result += (GetPrev(ind1) - GetPrev(ind2)) * (float) k;
    }
    float coef = 3.0f / (m * (m + 1.0f) * (2.0f * m + 1.0f));
    result = result * coef;
    return result;
}
