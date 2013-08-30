#pragma once
#include "common.hpp"
#include "SensorFilter.hpp"
#include <OVR.h>

struct MessageBodyFrame {
    glm::vec3 Acceleration;
    glm::vec3 RotationRate;
    glm::vec3 MagneticField;
    float TimeDelta;
    float Temperature;
};

class SensorFusion {
public:
    enum {
        MagMaxReferences = 80
    };

    typedef boost::unique_lock<boost::mutex> Lock;
    mutable boost::mutex mutex;
public:
    int LastTimestamp;
    int LastSampleCount;
    MessageBodyFrame currentMessage;


    void SetGravityEnabled(bool enableGravity);
    bool IsGravityEnabled() const;
    void SetYawCorrectionEnabled(bool enableYawCorrection);
    bool IsYawCorrectionEnabled() const;
    // Yaw correction is currently working (forcing a corrective yaw rotation)
    bool IsYawCorrectionInProgress() const;
    // Store the calibration matrix for the magnetometer
    void SetMagCalibration(const glm::mat4 & m);
    // True only if the mag has calibration values stored
    bool HasMagCalibration() const;
    // Force the mag into the uncalibrated state
    void ClearMagCalibration();

    // These refer to reference points that associate mag readings with orientations
    void ClearMagReferences();
    void SetMagRefDistance(const float d);
    // Obtain the current accumulated orientation.
    glm::quat GetOrientation() const;
    // Use a predictive filter to estimate the future orientation
    glm::quat GetPredictedOrientation(float pdt); // Specify lookahead time in ms
    glm::quat GetPredictedOrientation();
    // Obtain the last absolute acceleration reading, in m/s^2.
    glm::vec3 GetAcceleration() const;

    // Obtain the last angular velocity reading, in rad/s.
    glm::vec3 GetAngularVelocity() const;
    // Obtain the last magnetometer reading, in Gauss
    glm::vec3 GetMagnetometer() const;
    // Obtain the filtered magnetometer reading, in Gauss
    glm::vec3 GetFilteredMagnetometer() const;
    // Obtain the calibrated magnetometer reading (direction and field strength)
    glm::vec3 GetCalibratedMagnetometer() const;
    glm::vec3 GetCalibratedMagValue(const glm::vec3& rawMag) const;
    float GetMagRefYaw() const;
    float GetYawErrorAngle() const;
    // For later
    //glm::vec3    GetGravity() const;
    // Resets the current orientation
    void Reset();

    // Configuration
    // Gain used to correct gyro with accel. Default value is appropriate for typical use.
    float GetAccelGain() const;
    void SetAccelGain(float ag);
    // Multiplier for yaw rotation (turning); setting this higher than 1 (the default) can allow the game
    // to be played without auxillary rotation controls, possibly making it more immersive. Whether this is more
    // or less likely to cause motion sickness is unknown.
    float GetYawMultiplier() const;
    void SetYawMultiplier(float y);

    // Prediction functions.
    // Prediction delta specifes how much prediction should be applied in seconds; it should in
    // general be under the average rendering latency. Call GetPredictedOrientation() to get
    // predicted orientation.
    float GetPredictionDelta() const;
    void SetPrediction(float dt, bool enable = true);
    void SetPredictionEnabled(bool enable = true);
    bool IsPredictionEnabled();
    // Set the magnetometer's reference orientation for use in yaw correction
    // The supplied mag is an uncalibrated value
    void SetMagReference(const glm::quat& q, const glm::vec3& rawMag);
    // Default to current HMD orientation
    void SetMagReference();

    glm::quat Q;
    glm::quat QUncorrected;
    glm::vec3 A;
    glm::vec3 AngV;
    glm::vec3 CalMag;
    glm::vec3 RawMag;
    unsigned int Stage;
    float RunningTime;
    float DeltaT;
    float Gain;
    float YawMult;
    volatile bool EnableGravity;

    bool EnablePrediction;
    float PredictionDT;
    float PredictionTimeIncrement;

    SensorFilter FRawMag;
    SensorFilter FAccW;
    SensorFilter FAngV;

    int TiltCondCount;
    float TiltErrorAngle;
    glm::vec3 TiltErrorAxis;

    bool EnableYawCorrection;
    glm::mat4 MagCalibrationMatrix;
    bool MagCalibrated;
    int MagCondCount;
    float MagRefDistance;
    glm::quat MagRefQ;
    glm::vec3 MagRefM;
    float MagRefYaw;
    bool MagHasNearbyReference;
    glm::quat MagRefTableQ[MagMaxReferences];
    glm::vec3 MagRefTableM[MagMaxReferences];
    float MagRefTableYaw[MagMaxReferences];
    int MagNumReferences;
    float YawErrorAngle;
    int YawErrorCount;
    bool YawCorrectionInProgress;
    bool YawCorrectionActivated;


    static void AccelFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update, uint8_t sampleNumber);

    static void MagFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update);

    static void EulerFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update, uint8_t sampleNumber);

    void handleMessage(const MessageBodyFrame& msg);
    void processTrackerData(const OvrSensorMessage & s);
};
