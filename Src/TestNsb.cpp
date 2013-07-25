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

///////////////////////////////////////////////////////////////////////////////
// Sensor reports data in the following coordinate system:
// Accelerometer: 10^-4 m/s^2; X forward, Y right, Z Down.
// Gyro:          10^-4 rad/s; X positive roll right, Y positive pitch up; Z positive yaw right.

// We need to convert it to the following RHS coordinate system:
// X right, Y Up, Z Back (out of screen)
///////////////////////////////////////////////////////////////////////////////

void AccelFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update, uint8_t sampleNumber) {
    result.x = update.Samples[sampleNumber].AccelX;
    result.y = update.Samples[sampleNumber].AccelY;
    result.z = update.Samples[sampleNumber].AccelZ;
    result *= 0.0001f;
}

void MagFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update) {
    result.x = update.MagX;
    result.y = update.MagY;
    result.z = update.MagZ;
    result *= 0.0001f;
}

void EulerFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update, uint8_t sampleNumber) {
    result.x = update.Samples[sampleNumber].GyroX;
    result.y = update.Samples[sampleNumber].GyroY;
    result.z = update.Samples[sampleNumber].GyroZ;
    result *= 0.0001f;
}

struct MessageBodyFrame {
    glm::vec3 Acceleration;
    glm::vec3 RotationRate;
    glm::vec3 MagneticField;
    uint32_t TimeDelta;
    float Temperature;
};

struct SensorFusion {
    uint32_t LastTimestamp;
    uint8_t LastSampleCount;
    MessageBodyFrame currentMessage;

    glm::vec3 AngV;
    glm::vec3 A;
    glm::quat Q, QP;
    bool EnablePrediction;
    bool EnableGravity;
    float PredictionDT;
    float YawMult;
    float Gain;

    void vec3_scale(const glm::vec3 & source, float scale, glm::vec3 & target) {
        target = source;
        target *= scale;
    }

    float vec3_length(const glm::vec3 & source) {
        return source.length();
    }

    void vec3_set(const glm::vec3 & source, glm::vec3 & target) {
        target = source;
    }

    float vec3_angle(const glm::vec3 & a, glm::vec3 & b) {
        return 0;
    }

    //quat_multiply(Q, dQ, 0);
    //Q =  Q * dQ;
    void quat_multiply(glm::quat & a, const glm::quat & b, float something) {
        a = a * b;
    }

    void GetAngVFilterVal(glm::vec3 & target) {

    }

    void updateOrientation() {
        AngV = currentMessage.RotationRate;
        AngV.y *= YawMult;
        A = currentMessage.Acceleration;
        A *= currentMessage.TimeDelta;

        // Integration based on exact movement on 4D unit quaternion sphere.
        // Developed by Steve & Anna, this technique is based on Lie algebra
        // and exponential map.
        glm::vec3 dV;
        vec3_scale(AngV, currentMessage.TimeDelta, dV);
        //vec3_t    dV    = AngV * currentMessage.TimeDelta;
        const float angle = vec3_length(dV);  // Magnitude of angular velocity.

        if (angle > 0.0f) {
            float halfa = angle * 0.5f;
            float sina = sin(halfa) / angle;
            dV *= sina;
            glm::quat dQ(dV.x, dV.y, dV.z, cos(halfa)); // quat_t
            Q =  Q * dQ;

            if (EnablePrediction) {
                glm::vec3 AngVF;
                GetAngVFilterVal(AngVF);
                float angSpeed = vec3_length(AngVF);
                if (angSpeed > 0.001f) {
                    glm::vec3 axis = AngVF / angSpeed;
                    float halfaP = angSpeed * (currentMessage.TimeDelta + PredictionDT) * 0.5f;
                    float sinaP = sin(halfaP);
                    glm::quat dQP(axis.x*sinaP, axis.y*sinaP, axis.z*sinaP, cos(halfaP));
                    dQP[0] = axis.x * sinaP;
                    dQP[1] = axis.y * sinaP;
                    dQP[2] = axis.z * sinaP;
                    dQP[3] = cos(halfaP);
                    //dQP = quat_t();
                    QP =  Q * dQP;
                }
                else {
                    QP = Q;
                }
            }
            else {
                QP = Q;
                //QP = Q;
            }
        }

        // This introduces gravity drift adjustment based on gain
        float accelMagnitude = vec3_length(currentMessage.Acceleration);
        float angVMagnitude = vec3_length(AngV);
        const float gravityEpsilon = 0.4f;
        const float angVEpsilon = 3.0f; // Relatively slow rotation

        if (EnableGravity && (fabs(accelMagnitude - 9.81f) < gravityEpsilon) && (angVMagnitude < angVEpsilon)) {
            // TBD: Additional conditions:
            //  - Angular velocity < epsilon, or
            //  - Angle of transformed Acceleration < epsilon

            //printf("AC");

            static const glm::vec3 yUp(0, 1, 0);

            glm::vec3 aw;
            aw = Q * A;
            glm::quat q1 = glm::normalize(glm::quat(-aw.z * Gain, 0, aw.x * Gain, 1) * Q);
            float angle0 = vec3_angle(yUp, aw);
            //float    angle0 = yUp.Angle(aw);

            glm::vec3 temp = q1 * A;
            float angle1 = vec3_angle(yUp, temp);
            //float    angle1 = yUp.Angle(q1.Rotate(A));

            if (angle1 < angle0) {
                Q = q1;
            } else {
                glm::quat q2 = glm::normalize(glm::quat(aw.z * Gain, 0, -aw.x * Gain, 1) * Q);
                glm::vec3  temp2 = q2 * A;
                float angle2 = vec3_angle(yUp, temp2);
                if (angle2 < angle0) {
                    Q = q2;
                }
            }
        }
    }

    void processTrackerData(const OvrSensorMessage & s) {
        static const float timeUnit = (1.0f / 1000.f);

        unsigned long timestampDelta;

        if (s.Timestamp < LastTimestamp)
            timestampDelta = ((((int) s.Timestamp) + 0x10000) - (int) LastTimestamp);
        else
            timestampDelta = (s.Timestamp - LastTimestamp);

        // If we missed a small number of samples, replicate the last sample.
        if ((timestampDelta > LastSampleCount) && (timestampDelta <= 254)) {
            currentMessage.TimeDelta = (timestampDelta - LastSampleCount) * timeUnit;
            // TODO - Send faked update to listener
            updateOrientation();
        }

        LastSampleCount = s.SampleCount;
        LastTimestamp = s.Timestamp;
        size_t iterations = std::min((size_t) s.SampleCount, (size_t) 3);
        currentMessage.TimeDelta = timeUnit;
        if (s.SampleCount > 3) {
            currentMessage.TimeDelta *= (s.SampleCount - 2);
        }

        for (size_t i = 0; i < iterations; i++) {
            AccelFromBodyFrameUpdate(currentMessage.Acceleration, s, i);
            EulerFromBodyFrameUpdate(currentMessage.RotationRate, s, i);
            MagFromBodyFrameUpdate(currentMessage.MagneticField, s);
            currentMessage.Temperature = s.Temperature * 0.01f;
            // Update our orientation
            updateOrientation();
            // TimeDelta for the last two sample is always fixed.
            currentMessage.TimeDelta = timeUnit;
        }
    }
};

// hexstr [in] - string with hex encoded bytes
// bytes [out] - output in binary form
void hex_to_bin(std::string const& hexstr, ewkb_t & bytes) {
    bytes.clear();
    bytes.reserve(hexstr.size() / 2);
    for (std::string::size_type i = 0; i < hexstr.size() / 2; ++i) {
        std::istringstream iss(hexstr.substr(i * 2, 2));
        unsigned int n;
        iss >> std::hex >> n;
        bytes.push_back(static_cast<unsigned char>(n));
    }
}

void callback(const OvrSensorMessage * message) {
    static SensorFusion sf;
    sf.processTrackerData(*message);
    cout << message->Samples[0].AccelY << endl;
}

int test_nsb_main(int argc, char ** argv) {
    OVR_HANDLE handle = ovrOpenRiftRecording("/home/bdavis/eclipse/OculusSDK/Samples/data/rift1.json");
    sleep(1);
    ovrRegisterSampleHandler(handle, callback);
    sleep(1000);

    return 0;
}
