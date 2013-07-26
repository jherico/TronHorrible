#pragma once

#include <memory.h>

namespace Encom13 {
    namespace Stereo {

        class HMDInfo {
        public:
            // Size of the entire screen, in pixels.
            unsigned HResolution, VResolution;
            // Physical dimensions of the active screen in meters. Can be used to calculate
            // projection center while considering IPD.
            float HScreenSize, VScreenSize;
            // Physical offset from the top of the screen to the eye center, in meters.
            // This will usually, but not necessarily be half of VScreenSize.
            float VScreenCenter;
            // Distance from the eye to screen surface, in meters.
            // Useful for calculating FOV and projection.
            float EyeToScreenDistance;
            // Distance between physical lens centers useful for calculating distortion center.
            float LensSeparationDistance;
            // Configured distance between the user's eye centers, in meters. Defaults to 0.064.
            float InterpupillaryDistance;

            // Radial distortion correction coefficients.
            // The distortion assumes that the input texture coordinates will be scaled
            // by the following equation:
            //   uvResult = uvInput * (K0 + K1 * uvLength^2 + K2 * uvLength^4)
            // Where uvInput is the UV vector from the center of distortion in direction
            // of the mapped pixel, uvLength is the magnitude of that vector, and uvResult
            // the corresponding location after distortion.
            float DistortionK[4];

            float ChromaAbCorrection[4];

            // Desktop coordinate position of the screen (can be negative; may not be present on all platforms)
            int DesktopX, DesktopY;

            HMDInfo() :
                    HResolution(0), VResolution(0), HScreenSize(0), VScreenSize(0), VScreenCenter(0),
                            EyeToScreenDistance(0), LensSeparationDistance(0), InterpupillaryDistance(0), DesktopX(0),
                            DesktopY(0) {
                memset(DistortionK, 0, sizeof(DistortionK));
                DistortionK[0] = 1;
                ChromaAbCorrection[0] = ChromaAbCorrection[2] = 1;
                ChromaAbCorrection[1] = ChromaAbCorrection[3] = 0;
            }
        };

        enum StereoMode {
            Stereo_None = 0, Stereo_LeftRight_Multipass = 1
        };

// StereoEye specifies which eye we are rendering for; it is used to
// retrieve StereoEyeParams.
        enum StereoEye {
            StereoEye_Center, StereoEye_Left, StereoEye_Right
        };

        struct Viewport {
            int x, y;
            int w, h;

            Viewport() {
            }
            Viewport(int x1, int y1, int w1, int h1) :
                    x(x1), y(y1), w(w1), h(h1) {
            }

            bool operator ==(const Viewport& vp) const {
                return (x == vp.x) && (y == vp.y) && (w == vp.w) && (h == vp.h);
            }
            bool operator !=(const Viewport& vp) const {
                return !operator ==(vp);
            }
        };

        class DistortionConfig {
        public:
            DistortionConfig(float k0 = 1.0f, float k1 = 0.0f, float k2 = 0.0f, float k3 = 0.0f) :
                    XCenterOffset(0), YCenterOffset(0), Scale(1.0f) {
                SetCoefficients(k0, k1, k2, k3);
                SetChromaticAberration();
            }

            void SetCoefficients(float k0, float k1 = 0.0f, float k2 = 0.0f, float k3 = 0.0f) {
                K[0] = k0;
                K[1] = k1;
                K[2] = k2;
                K[3] = k3;
            }

            void SetChromaticAberration(float red1 = 1.0f, float red2 = 0.0f, float blue1 = 1.0f, float blue2 = 0.0f) {
                ChromaticAberration[0] = red1;
                ChromaticAberration[1] = red2;
                ChromaticAberration[2] = blue1;
                ChromaticAberration[3] = blue2;
            }

            // DistortionFn applies distortion equation to the argument. The returned
            // value should match distortion equation used in shader.
            float DistortionFn(float r) const {
                float rsq = r * r;
                float scale = r * (K[0] + K[1] * rsq + K[2] * rsq * rsq + K[3] * rsq * rsq * rsq);
                return scale;
            }

            // DistortionFnInverse computes the inverse of the distortion function on an argument.
            float DistortionFnInverse(float r);

            float K[4];
            float XCenterOffset, YCenterOffset;
            float Scale;

            float ChromaticAberration[4]; // Additional per-channel scaling is applied after distortion:
                                          // Index [0] - Red channel constant coefficient.
                                          // Index [1] - Red channel r^2 coefficient.
                                          // Index [2] - Blue channel constant coefficient.
                                          // Index [3] - Blue channel r^2 coefficient.
        };

//-----------------------------------------------------------------------------------
// ***** StereoEyeParams

// StereoEyeParams describes RenderDevice configuration needed to render
// the scene for one eye.
        class StereoEyeParams {
        public:
            StereoEye Eye;
            Viewport VP;               // Viewport that we are rendering to
            const DistortionConfig* pDistortion;

            glm::mat4 ViewAdjust; // Translation to be applied to view matrix.
            glm::mat4 Projection; // Projection matrix used with this eye.
            glm::mat4 OrthoProjection; // Orthographic projection used with this eye.

            void Init(StereoEye eye, const Viewport &vp, float vofs, const glm::mat4& proj,
                    const glm::mat4 & orthoProj, const DistortionConfig* distortion = 0) {
                Eye = eye;
                VP = vp;
                ViewAdjust = glm::translate(glm::mat4(), glm::vec3(vofs, 0, 0));
                Projection = proj;
                OrthoProjection = orthoProj;
                pDistortion = distortion;
            }
        };

//-----------------------------------------------------------------------------------
// *****  StereoConfig

// StereoConfig maintains a scene stereo state and allow switching between different
// stereo rendering modes. To support rendering, StereoConfig keeps track of HMD
// variables such as screen size, eye-to-screen distance and distortion, and computes
// extra data such as FOV and distortion center offsets based on it. Rendering
// parameters are returned though StereoEyeParams for each eye.
//
// Beyond regular 3D projection, this class supports rendering a 2D orthographic
// surface for UI and text. The 2D surface will be defined as fitting within a 2D
// field of view (85 degrees by default) and used [-1,1] coordinate system with
// square pixels. The (0,0) coordinate corresponds to eye center location
// that is properly adjusted during rendering through SterepRenderParams::Adjust2D.
// Genreally speaking, text outside [-1,1] coordinate range will not be readable.

        class StereoConfig {
        public:

            StereoConfig(StereoMode mode = Stereo_LeftRight_Multipass,
                    const Viewport& fullViewport = Viewport(0, 0, 1280, 800));

            // *** Modifiable State Access

            // Sets a stereo rendering mode and updates internal cached
            // state (matrices, per-eye view) based on it.
            void SetStereoMode(StereoMode mode) {
                Mode = mode;
                DirtyFlag = true;
            }
            StereoMode GetStereoMode() const {
                return Mode;
            }

            // Sets HMD parameters; also initializes distortion coefficients.
            void SetHMDInfo(const HMDInfo& hmd);
            const HMDInfo& GetHMDInfo() const {
                return HMD;
            }

            // Query physical eye-to-screen distance in meters, which combines screen-to-lens and
            // and lens-to-eye pupil distances. Modifying this value adjusts FOV.
            float GetEyeToScreenDistance() const {
                return HMD.EyeToScreenDistance;
            }
            void SetEyeToScreenDistance(float esd) {
                HMD.EyeToScreenDistance = esd;
                DirtyFlag = true;
            }

            // Interpupillary distance used for stereo, in meters. Default is 0.064m (64 mm).
            void SetIPD(float ipd) {
                InterpupillaryDistance = ipd;
                IPDOverride = DirtyFlag = true;
            }
            float GetIPD() const {
                return InterpupillaryDistance;
            }

            // Set full render target viewport; for HMD this includes both eyes.
            void SetFullViewport(const Viewport& vp);
            const Viewport& GetFullViewport() const {
                return FullView;
            }

            // Aspect ratio defaults to ((w/h)*multiplier) computed per eye.
            // Aspect multiplier allows adjusting aspect ratio consistently for Stereo/NoStereo.
            void SetAspectMultiplier(float m) {
                AspectMultiplier = m;
                DirtyFlag = true;
            }
            float GetAspectMultiplier() const {
                return AspectMultiplier;
            }

            // For the distorted image to fill rendered viewport, input texture render target needs to be
            // scaled by DistortionScale before sampling. The scale factor is computed by fitting a point
            // on of specified radius from a distortion center, more easily specified as a coordinate.
            // SetDistortionFitPointVP sets the (x,y) coordinate of the point that scale will be "fit" to,
            // assuming [-1,1] coordinate range for full left-eye viewport. A fit point is a location
            // where source (pre-distortion) and target (post-distortion) image match each other.
            // For the right eye, the interpretation of 'u' will be inverted.
            void SetDistortionFitPointVP(float x, float y);
            // SetDistortionFitPointPixels sets the (x,y) coordinate of the point that scale will be "fit" to,
            // specified in pixeld for full left-eye texture.
            void SetDistortionFitPointPixels(float x, float y);

            // Changes all distortion settings.
            // Note that setting HMDInfo also changes Distortion coefficients.
            void SetDistortionConfig(const DistortionConfig& d) {
                Distortion = d;
                DirtyFlag = true;
            }

            // Modify distortion coefficients; useful for adjustment tweaking.
            void SetDistortionK(int i, float k) {
                Distortion.K[i] = k;
                DirtyFlag = true;
            }
            float GetDistortionK(int i) const {
                return Distortion.K[i];
            }

            // Sets the fieldOfView that the 2D coordinate area stretches to.
            void Set2DAreaFov(float fovRadians);

            // *** Computed State

            // Return current aspect ratio.
            float GetAspect() {
                updateIfDirty();
                return Aspect;
            }

            // Return computed vertical FOV in radians/degrees.
            float GetYFOVRadians() {
                updateIfDirty();
                return YFov;
            }

            float GetYFOVDegrees() {
                return glm::degrees(GetYFOVRadians());
            }

            // Query horizontal projection center offset as a distance away from the
            // one-eye [-1,1] unit viewport.
            // Positive return value should be used for left eye, negative for right eye.
            float GetProjectionCenterOffset() {
                updateIfDirty();
                return ProjectionCenterOffset;
            }

            // GetDistortionConfig isn't const because XCenterOffset bay need to be recomputed.
            const DistortionConfig& GetDistortionConfig() {
                updateIfDirty();
                return Distortion;
            }

            // Returns DistortionScale factor by which input texture size is increased to make
            // post-distortion result distortion fit the viewport.
            float GetDistortionScale() {
                updateIfDirty();
                return Distortion.Scale;
            }

            // Returns the size of a pixel within 2D coordinate system.
            float Get2DUnitPixel() {
                updateIfDirty();
                return (2.0f / (FovPixels * Distortion.Scale));
            }

            // Returns full set of Stereo rendering parameters for the specified eye.
            const StereoEyeParams& GetEyeRenderParams(StereoEye eye);

        private:

            void updateIfDirty() {
                if (DirtyFlag)
                    updateComputedState();
            }
            void updateComputedState();

            void updateDistortionOffsetAndScale();
            void updateProjectionOffset();
            void update2D();
            void updateEyeParams();

            // *** Modifiable State

            StereoMode Mode;
            float InterpupillaryDistance;
            float AspectMultiplier;        // Multiplied into aspect ratio to change it.
            HMDInfo HMD;
            DistortionConfig Distortion;
            float DistortionFitX, DistortionFitY; // In [-1,1] half-screen viewport units.
            Viewport FullView;                       // Entire window viewport.

            float Area2DFov;                    // FOV range mapping to [-1, 1] 2D area.

            // *** Computed State

            bool DirtyFlag;   // Set when any if the modifiable state changed.
            bool IPDOverride; // True after SetIPD was called.
            float YFov;        // Vertical FOV.
            float Aspect;      // Aspect ratio: (w/h)*AspectMultiplier.
            float ProjectionCenterOffset;
            StereoEyeParams EyeRenderParams[2];

            // ** 2D Rendering

            // Number of 2D pixels in the FOV. This defines [-1,1] coordinate range for 2D.
            float FovPixels;
            glm::mat4 OrthoCenter;
            float OrthoPixelOffset;
        };

    }
}
