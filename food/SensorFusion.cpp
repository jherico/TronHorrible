#include "SensorFusion.hpp"

void SensorFusion::SetGravityEnabled(bool enableGravity) {
    EnableGravity = enableGravity;
}

bool SensorFusion::IsGravityEnabled() const {
    return EnableGravity;
}

void SensorFusion::SetYawCorrectionEnabled(bool enableYawCorrection) {
    EnableYawCorrection = enableYawCorrection;
}

bool SensorFusion::IsYawCorrectionEnabled() const {
    return EnableYawCorrection;
}

// Yaw correction is currently working (forcing a corrective yaw rotation)
bool SensorFusion::IsYawCorrectionInProgress() const {
    return YawCorrectionInProgress;
}

// Store the calibration matrix for the magnetometer
void SensorFusion::SetMagCalibration(const glm::mat4 & m) {
    MagCalibrationMatrix = m;
    MagCalibrated = true;
}

// True only if the mag has calibration values stored
bool SensorFusion::HasMagCalibration() const {
    return MagCalibrated;
}

// Force the mag into the uncalibrated state
void SensorFusion::ClearMagCalibration() {
    MagCalibrated = false;
}

// These refer to reference points that associate mag readings with orientations
void SensorFusion::ClearMagReferences() {
    MagNumReferences = 0;
}
void SensorFusion::SetMagRefDistance(const float d) {
    MagRefDistance = d;
}

// Obtain the current accumulated orientation.
glm::quat SensorFusion::GetOrientation() const {
    Lock lock(mutex);
    return Q;
}

// Use a predictive filter to estimate the future orientation
glm::quat SensorFusion::GetPredictedOrientation() {
    return GetPredictedOrientation(PredictionDT);
}

// Use a predictive filter to estimate the future orientation
glm::quat SensorFusion::GetPredictedOrientation(float dt) {
    Lock lock(mutex);
    return Q;
}

// Obtain the last absolute acceleration reading, in m/s^2.
glm::vec3 SensorFusion::GetAcceleration() const {
    Lock lock(mutex);
    return A;
}

// Obtain the last angular velocity reading, in rad/s.
glm::vec3 SensorFusion::GetAngularVelocity() const {
    Lock lock(mutex);
    return AngV;
}
// Obtain the last magnetometer reading, in Gauss
glm::vec3 SensorFusion::GetMagnetometer() const {
    Lock lock(mutex);
    return RawMag;
}
// Obtain the filtered magnetometer reading, in Gauss
glm::vec3 SensorFusion::GetFilteredMagnetometer() const {
    Lock lock(mutex);
    return FRawMag.Mean();
}
// Obtain the calibrated magnetometer reading (direction and field strength)
glm::vec3 SensorFusion::GetCalibratedMagnetometer() const {
    Lock lock(mutex);
    return CalMag;
}

float SensorFusion::GetMagRefYaw() const {
    return MagRefYaw;
}

float SensorFusion::GetYawErrorAngle() const {
    return YawErrorAngle;
}

// Resets the current orientation
void SensorFusion::Reset() {
    Q = glm::quat();
}

// Configuration

// Gain used to correct gyro with accel. Default value is appropriate for typical use.
float SensorFusion::GetAccelGain() const {
    return Gain;
}
void SensorFusion::SetAccelGain(float ag) {
    Gain = ag;
}

// Multiplier for yaw rotation (turning); setting this higher than 1 (the default) can allow the game
// to be played without auxillary rotation controls, possibly making it more immersive. Whether this is more
// or less likely to cause motion sickness is unknown.
float SensorFusion::GetYawMultiplier() const {
    return YawMult;
}
void SensorFusion::SetYawMultiplier(float y) {
    YawMult = y;
}

// Prediction functions.
// Prediction delta specifes how much prediction should be applied in seconds; it should in
// general be under the average rendering latency. Call GetPredictedOrientation() to get
// predicted orientation.
float SensorFusion::GetPredictionDelta() const {
    return PredictionDT;
}
void SensorFusion::SetPrediction(float dt, bool enable) {
    PredictionDT = dt;
    EnablePrediction = enable;
}
void SensorFusion::SetPredictionEnabled(bool enable) {
    EnablePrediction = enable;
}
bool SensorFusion::IsPredictionEnabled() {
    return EnablePrediction;
}

// Default to current HMD orientation
void SensorFusion::SetMagReference() {
    SetMagReference(Q, RawMag);
}

void SensorFusion::SetMagReference(const glm::quat & q, const glm::vec3 & mag) {
//    SetMagReference(Q, RawMag);
}

void SensorFusion::AccelFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update, uint8_t sampleNumber) {
    result.x = update.Samples[sampleNumber].AccelX;
    result.y = update.Samples[sampleNumber].AccelY;
    result.z = update.Samples[sampleNumber].AccelZ;
    result *= 0.0001f;
}

void SensorFusion::MagFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update) {
    result.x = update.MagX;
    result.y = update.MagY;
    result.z = update.MagZ;
    result *= 0.0001f;
}

void SensorFusion::EulerFromBodyFrameUpdate(glm::vec3 & result, const OvrSensorMessage & update, uint8_t sampleNumber) {
    result.x = update.Samples[sampleNumber].GyroX;
    result.y = update.Samples[sampleNumber].GyroY;
    result.z = update.Samples[sampleNumber].GyroZ;
    result *= 0.0001f;
}

void SensorFusion::handleMessage(const MessageBodyFrame& msg) {
    // Put the sensor readings into convenient local variables
    const glm::vec3 & angVel = msg.RotationRate;
    const glm::vec3 & rawAccel = msg.Acceleration;
    const glm::vec3 & mag = msg.MagneticField;

    // Set variables accessible through the class API
    DeltaT = msg.TimeDelta;
    AngV = msg.RotationRate;
    AngV.y *= YawMult;  // Warning: If YawMult != 1, then AngV is not true angular velocity
    A = rawAccel;

    // Allow external access to uncalibrated magnetometer values
    RawMag = mag;

//        // Apply the calibration parameters to raw mag
//        if (HasMagCalibration()) {
//            mag.x += MagCalibrationMatrix.M[0][3];
//            mag.y += MagCalibrationMatrix.M[1][3];
//            mag.z += MagCalibrationMatrix.M[2][3];
//        }
//
    // Provide external access to calibrated mag values
    // (if the mag is not calibrated, then the raw value is returned)
    CalMag = mag;

    float angVelLength = glm::length(angVel);
    float accLength = glm::length(rawAccel);

    // Acceleration in the world frame (Q is current HMD orientation)
    glm::vec3 accWorld = Q * rawAccel;

    // Keep track of time
    Stage++;
    RunningTime += DeltaT;

    // Insert current sensor data into filter history
    FRawMag.AddElement(RawMag);
    FAccW.AddElement(accWorld);
    FAngV.AddElement(angVel);

    // Update orientation Q based on gyro outputs.  This technique is
    // based on direct properties of the angular velocity vector:
    // Its direction is the current rotation axis, and its magnitude
    // is the rotation rate (rad/sec) about that axis.  Our sensor
    // sampling rate is so fast that we need not worry about integral
    // approximation error (not yet, anyway).
    if (angVelLength > 0.0f) {
        glm::vec3 rotAxis = angVel / angVelLength;
        float halfRotAngle = angVelLength * DeltaT * 0.5f;
        float sinHRA = sin(halfRotAngle);
        glm::quat deltaQ(rotAxis.x * sinHRA, rotAxis.y * sinHRA, rotAxis.z * sinHRA, cos(halfRotAngle));

        Q = Q * deltaQ;
    }

    // The quaternion magnitude may slowly drift due to numerical error,
    // so it is periodically normalized.
    if (Stage % 5000 == 0)
        Q = glm::normalize(Q);

    // Maintain the uncorrected orientation for later use by predictive filtering
    QUncorrected = Q;

    // Perform tilt correction using the accelerometer data. This enables
    // drift errors in pitch and roll to be corrected. Note that yaw cannot be corrected
    // because the rotation axis is parallel to the gravity vector.
    if (EnableGravity) {
        // Correcting for tilt error by using accelerometer data
        const float gravityEpsilon = 0.4f;
        const float angVelEpsilon = 0.1f; // Relatively slow rotation
        const int tiltPeriod = 50;   // Required time steps of stability
        const float maxTiltError = 0.05f;
        const float minTiltError = 0.01f;

        // This condition estimates whether the only measured acceleration is due to gravity
        // (the Rift is not linearly accelerating).  It is often wrong, but tends to average
        // out well over time.
        if ((fabs(accLength - 9.81f) < gravityEpsilon) && (angVelLength < angVelEpsilon))
            TiltCondCount++;
        else
            TiltCondCount = 0;

        // After stable measurements have been taken over a sufficiently long period,
        // estimate the amount of tilt error and calculate the tilt axis for later correction.
        if (TiltCondCount >= tiltPeriod) {   // Update TiltErrorEstimate
            TiltCondCount = 0;
            // Use an average value to reduce noise (could alternatively use an LPF)
            glm::vec3 accWMean = FAccW.Mean();
            // Project the acceleration vector into the XZ plane
            glm::vec3 xzAcc = glm::vec3(accWMean.x, 0.0f, accWMean.z);
            // The unit normal of xzAcc will be the rotation axis for tilt correction
            glm::vec3 tiltAxis = glm::normalize(glm::vec3(xzAcc.z, 0.0f, -xzAcc.x));
            glm::vec3 yUp = glm::vec3(0.0f, 1.0f, 0.0f);
            // This is the amount of rotation
            float tiltAngle = glm::angle(yUp, accWMean);
            // Record values if the tilt error is intolerable
            if (tiltAngle > maxTiltError) {
                TiltErrorAngle = tiltAngle;
                TiltErrorAxis = tiltAxis;
            }
        }

        // This part performs the actual tilt correction as needed
        if (TiltErrorAngle > minTiltError) {
            if ((TiltErrorAngle > 0.4f) && (RunningTime < 8.0f)) {   // Tilt completely to correct orientation
                Q = glm::angleAxis(-TiltErrorAngle, TiltErrorAxis) * Q;
                TiltErrorAngle = 0.0f;
            } else {
                //LogText("Performing tilt correction  -  Angle: %f   Axis: %f %f %f\n",
                //        TiltErrorAngle,TiltErrorAxis.x,TiltErrorAxis.y,TiltErrorAxis.z);
                //float deltaTiltAngle = -Gain*TiltErrorAngle*0.005f;
                // This uses aggressive correction steps while your head is moving fast
                float deltaTiltAngle = -Gain * TiltErrorAngle * 0.005f * (5.0f * angVelLength + 1.0f);
                // Incrementally "un-tilt" by a small step size
                Q = glm::angleAxis(deltaTiltAngle, TiltErrorAxis) * Q;
                TiltErrorAngle += deltaTiltAngle;
            }
        }
    }

//        // Yaw drift correction based on magnetometer data.  This corrects the part of the drift
//        // that the accelerometer cannot handle.
//        // This will only work if the magnetometer has been enabled, calibrated, and a reference
//        // point has been set.
//        static const float maxAngVelLength = 3.0f;
//        static const int magWindow = 5;
//        static const float yawErrorMax = 0.1f;
//        static const float yawErrorMin = 0.01f;
//        static const int yawErrorCountLimit = 50;
//        static const float yawRotationStep = 0.00002f;
//
//        if (angVelLength < maxAngVelLength)
//            MagCondCount++;
//        else
//            MagCondCount = 0;
//
//        // Find, create, and utilize reference points for the magnetometer
//        // Need to be careful not to set reference points while there is significant tilt error
//        if ((EnableYawCorrection && MagCalibrated) && (RunningTime > 10.0f) && (TiltErrorAngle < 0.2f)) {
//            if (MagNumReferences == 0) {
//                SetMagReference(); // Use the current direction
//            } else if (Q.Distance(MagRefQ) > MagRefDistance) {
//                MagHasNearbyReference = false;
//                float bestDist = 100000.0f;
//                int bestNdx = 0;
//                float dist;
//                for (int i = 0; i < MagNumReferences; i++) {
//                    dist = Q.Distance(MagRefTableQ[i]);
//                    if (dist < bestDist) {
//                        bestNdx = i;
//                        bestDist = dist;
//                    }
//                }
//
//                if (bestDist < MagRefDistance) {
//                    MagHasNearbyReference = true;
//                    MagRefQ = MagRefTableQ[bestNdx];
//                    MagRefM = MagRefTableM[bestNdx];
//                    MagRefYaw = MagRefTableYaw[bestNdx];
//                    //LogText("Using reference %d\n",bestNdx);
//                } else if (MagNumReferences < MagMaxReferences)
//                    SetMagReference();
//            }
//        }
//
//        YawCorrectionInProgress = false;
//        if (EnableYawCorrection && MagCalibrated && (RunningTime > 2.0f) && (MagCondCount >= magWindow)
//                && MagHasNearbyReference) {
//            // Use rotational invariance to bring reference mag value into global frame
//            glm::vec3 grefmag = MagRefQ.Rotate(GetCalibratedMagValue(MagRefM));
//            // Bring current (averaged) mag reading into global frame
//            glm::vec3 gmag = Q.Rotate(GetCalibratedMagValue(FRawMag.Mean()));
//            // Calculate the reference yaw in the global frame
//            Anglef gryaw = Anglef(atan2(grefmag.x, grefmag.z));
//            // Calculate the current yaw in the global frame
//            Anglef gyaw = Anglef(atan2(gmag.x, gmag.z));
//            // The difference between reference and current yaws is the perceived error
//            Anglef YawErrorAngle = gyaw - gryaw;
//
//            //LogText("Yaw error estimate: %f\n",YawErrorAngle.Get());
//            // If the perceived error is large, keep count
//            if ((YawErrorAngle.Abs() > yawErrorMax) && (!YawCorrectionActivated))
//                YawErrorCount++;
//            // After enough iterations of high perceived error, start the correction process
//            if (YawErrorCount > yawErrorCountLimit)
//                YawCorrectionActivated = true;
//            // If the perceived error becomes small, turn off the yaw correction
//            if ((YawErrorAngle.Abs() < yawErrorMin) && YawCorrectionActivated) {
//                YawCorrectionActivated = false;
//                YawErrorCount = 0;
//            }
//
//            // Perform the actual yaw correction, due to previously detected, large yaw error
//            if (YawCorrectionActivated) {
//                YawCorrectionInProgress = true;
//                // Incrementally "unyaw" by a small step size
//                Q = glm::quat(glm::vec3(0.0f, 1.0f, 0.0f), -yawRotationStep * YawErrorAngle.Sign()) * Q;
//            }
//        }
}

void SensorFusion::processTrackerData(const OvrSensorMessage & s) {
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
        handleMessage(currentMessage);
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
        handleMessage(currentMessage);
        // TimeDelta for the last two sample is always fixed.
        currentMessage.TimeDelta = timeUnit;
    }
}

