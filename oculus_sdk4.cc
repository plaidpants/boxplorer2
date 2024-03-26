// 2013 Dominic Szablewski - phoboslab.org
// sdk4 mods by marius

#include "oculus_sdk4.h"
//#include "OVR.h"

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

#pragma comment(lib, "libovr.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846f // matches value in gcc v2 math.h
#endif

// Some libs that OVR needs..
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gdi32.lib")

//static ovrHmd hmd = NULL;
static ovrSession hmd;
static ovrGraphicsLuid hmd2;

int InitOculusSDK() {
//	ovr_Initialize();
//	hmd = ovrHmd_Create(0);
//	ovr_Initialize(0);

    ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
    ovrResult result = ovr_Initialize(&initParams);

	//	ovrResult  result = ovrHmd_Create(0, &hmd);
	result = ovr_Create(&hmd, &hmd2);

  if (!hmd) {
    return 0;
  } else {
//    ovrSizei resolution = hmd->Resolution;
//    printf("HMD rez %dx%d\n", resolution.w, resolution.h);
  }

//  ovrHmd_SetEnabledCaps(hmd,
//      ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

//  ovrHmd_ConfigureTracking(hmd,
//      ovrTrackingCap_Orientation
//      | ovrTrackingCap_MagYawCorrection
//      | ovrTrackingCap_Position, 0);
  
  return 1;
}

#if 0
void GetOculusView(float view[3]) {
  // GetPredictedOrientation() works even if prediction is disabled
  OVR::Quatf q = fusion->GetPredictedOrientation();
  
  q.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&view[1], &view[0], &view[2]);

  view[0] = (-view[0] * 180.0f) / M_PI;
  view[1] = (view[1] * 180.0f) / M_PI;
  view[2] = (-view[2] * 180.0f) / M_PI
}
#endif

#include <cmath> // For atan2, asin

// Converts a quaternion to Euler angles (pitch, yaw, roll)
void QuatToEulerAngles(const ovrQuatf& q, float& pitch, float& yaw, float& roll) {
    // Roll (x-axis rotation)
    double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    roll = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (y-axis rotation)
    double sinp = 2.0 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        pitch = std::asin(sinp);

    // Yaw (z-axis rotation)
    double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    yaw = std::atan2(siny_cosp, cosy_cosp);
}

void GetOculusView(float view[3]) {
    if (!hmd) return;

    ovrTrackingState ts = ovr_GetTrackingState(hmd, ovr_GetTimeInSeconds(), ovrTrue);
    if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
        float yaw, pitch, roll;
        // Directly pass the quaternion object without using the address-of operator
        QuatToEulerAngles(ts.HeadPose.ThePose.Orientation, pitch, yaw, roll);

        // Convert radians to degrees
        view[0] = yaw * (180.0f / M_PI);
        view[1] = pitch * (180.0f / M_PI);
        view[2] = roll * (180.0f / M_PI);
    }
}

#if 0
bool GetOculusQuat(float quat[4]) {
  bool result = false;
//  ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, 0.0/*ovr_GetTimeInSeconds()*/);
//  if (ts.StatusFlags & (ovrStatus_OrientationTracked)) {
//    ovrPoseStatef poseState = ts.HeadPose;
//    ovrPosef posef = poseState.ThePose;
//    ovrQuatf q = posef.Orientation;
  
//    quat[0] = q.x;
//    quat[1] = q.y;
//    quat[2] = q.z;
//    quat[3] = q.w;

//    result = true;
//  }
#if 1
//  if (ts.StatusFlags & (ovrStatus_PositionTracked)) {
//    ovrPoseStatef poseState = ts.HeadPose;
//    ovrPosef posef = poseState.ThePose;
//    ovrVector3f p = posef.Position;

//    printf("pos(%f, %f, %f)\n", p.x, p.y, p.z);
//  }
#endif
  return result;
}
#endif

bool GetOculusQuat(float quat[4]) {
    // Ensure we have a valid session
    if (!hmd) return false;

    // Get the current tracking state
    ovrTrackingState ts = ovr_GetTrackingState(hmd, ovr_GetTimeInSeconds(), ovrTrue);

    // Check if orientation is tracked
    if (ts.StatusFlags & ovrStatus_OrientationTracked) {
        ovrQuatf q = ts.HeadPose.ThePose.Orientation;

        // Copy the quaternion components to the output parameter
        quat[0] = q.x;
        quat[1] = q.y;
        quat[2] = q.z;
        quat[3] = q.w;

        // Optionally, log the position if it's tracked
#if 0 // This preprocessor directive seems to control additional logging/debugging
        if (ts.StatusFlags & ovrStatus_PositionTracked) {
            ovrVector3f p = ts.HeadPose.ThePose.Position;
            printf("Position: (%f, %f, %f)\n", p.x, p.y, p.z);
        }
#endif

        return true; // Orientation was successfully obtained and copied
    }

    return false; // Orientation tracking was not available
}



void ReleaseOculusSDK()
{
  if (hmd) {
//    ovrHmd_Destroy(hmd);
    ovr_Destroy(hmd);
    hmd = NULL;
  }
  ovr_Shutdown();
}

void SetOculusPrediction(float time) {
}

#if 0
int GetOculusDeviceInfo(hmd_settings_t *hmd_settings) {
//  hmd_settings->h_resolution = hmd->Resolution.w;
//  hmd_settings->v_resolution = hmd->Resolution.h;

  hmd_settings->interpupillary_distance = hmdinfo.InterpupillaryDistance;
  hmd_settings->lens_separation_distance = hmdinfo.LensSeparationDistance;
  hmd_settings->eye_to_screen_distance = hmdinfo.EyeToScreenDistance;

  memcpy(hmd_settings->distortion_k,
                  hmdinfo.DistortionK, sizeof(float) * 4);
  memcpy(hmd_settings->chrom_abr,
                  hmdinfo.ChromaAbCorrection, sizeof(float) * 4);

  return 1;
}
#endif

int GetOculusDeviceInfo(hmd_settings_t* hmd_settings) {
    if (!hmd || !hmd_settings) return 0;

    // Get the HMD description
    ovrHmdDesc desc = ovr_GetHmdDesc(hmd);

    // Resolution
    hmd_settings->h_resolution = desc.Resolution.w;
    hmd_settings->v_resolution = desc.Resolution.h;

    // The newer versions of the SDK may not directly provide some of the following info:
    // interpupillary_distance, lens_separation_distance, eye_to_screen_distance, distortion_k, chrom_abr
    // You might need to handle them differently or they might not be needed at all for newer API usage.

    // IPD can be obtained through ovr_GetFloat (if necessary for your application)
    hmd_settings->interpupillary_distance = ovr_GetFloat(hmd, "OVR_KEY_IPD", 0.064f); // Default value as example

    // For other parameters like lens separation, eye to screen distance, distortion coefficients, and chromatic aberration correction:
    // These are either managed automatically by the runtime or not exposed by the Oculus SDK anymore.
    // You may need to adjust your application to work without directly accessing these.

    return 1; // Indicate success
}

#if 0
void ResetOculusOrientation() {
//	ovr_RecenterTrackingOrigin(hmd);
//	ovr_RecenterPose(hmd);
//  ovrHmd_RecenterPose(hmd);
}
#endif

void ResetOculusOrientation() {
    if (!hmd) return;

    // Recenter the tracking origin for the session
    ovrResult result = ovr_RecenterTrackingOrigin(hmd);

    // Optionally, check the result to ensure recentering was successful
    if (OVR_SUCCESS(result)) {
        // Recentering succeeded
    }
    else {
        // Handle error, recentering failed
    }
}

#if 0
#include <SDL_opengl.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_main.h>
typedef Size<int> Sizei;

struct OculusTextureBuffer
{
    ovrSession          Session;
    ovrTextureSwapChain ColorTextureChain;
    ovrTextureSwapChain DepthTextureChain;
    GLuint              fboId;
    Sizei               texSize;

    OculusTextureBuffer(ovrSession session, Sizei size, int sampleCount) :
        Session(session),
        ColorTextureChain(nullptr),
        DepthTextureChain(nullptr),
        fboId(0),
        texSize(0, 0)
    {
        assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

        texSize = size;

        // This texture isn't necessarily going to be a rendertarget, but it usually is.
        assert(session); // No HMD? A little odd.

        ovrTextureSwapChainDesc desc = {};
        desc.Type = ovrTexture_2D;
        desc.ArraySize = 1;
        desc.Width = size.w;
        desc.Height = size.h;
        desc.MipLevels = 1;
        desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc.SampleCount = sampleCount;
        desc.StaticImage = ovrFalse;

        {
            ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &ColorTextureChain);

            int length = 0;
            ovr_GetTextureSwapChainLength(session, ColorTextureChain, &length);

            if (OVR_SUCCESS(result))
            {
                for (int i = 0; i < length; ++i)
                {
                    GLuint chainTexId;
                    ovr_GetTextureSwapChainBufferGL(Session, ColorTextureChain, i, &chainTexId);
                    glBindTexture(GL_TEXTURE_2D, chainTexId);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            }
        }

        desc.Format = OVR_FORMAT_D32_FLOAT;

        {
            ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &DepthTextureChain);

            int length = 0;
            ovr_GetTextureSwapChainLength(session, DepthTextureChain, &length);

            if (OVR_SUCCESS(result))
            {
                for (int i = 0; i < length; ++i)
                {
                    GLuint chainTexId;
                    ovr_GetTextureSwapChainBufferGL(Session, DepthTextureChain, i, &chainTexId);
                    glBindTexture(GL_TEXTURE_2D, chainTexId);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            }
        }

        glGenFramebuffers(1, &fboId);
    }

    ~OculusTextureBuffer()
    {
        if (ColorTextureChain)
        {
            ovr_DestroyTextureSwapChain(Session, ColorTextureChain);
            ColorTextureChain = nullptr;
        }
        if (DepthTextureChain)
        {
            ovr_DestroyTextureSwapChain(Session, DepthTextureChain);
            DepthTextureChain = nullptr;
        }
        if (fboId)
        {
            glDeleteFramebuffers(1, &fboId);
            fboId = 0;
        }
    }

    Sizei GetSize() const
    {
        return texSize;
    }

    void SetAndClearRenderSurface()
    {
        GLuint curColorTexId;
        GLuint curDepthTexId;
        {
            int curIndex;
            ovr_GetTextureSwapChainCurrentIndex(Session, ColorTextureChain, &curIndex);
            ovr_GetTextureSwapChainBufferGL(Session, ColorTextureChain, curIndex, &curColorTexId);
        }
        {
            int curIndex;
            ovr_GetTextureSwapChainCurrentIndex(Session, DepthTextureChain, &curIndex);
            ovr_GetTextureSwapChainBufferGL(Session, DepthTextureChain, curIndex, &curDepthTexId);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curColorTexId, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, curDepthTexId, 0);

        glViewport(0, 0, texSize.w, texSize.h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    void UnsetRenderSurface()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    void Commit()
    {
        ovr_CommitTextureSwapChain(Session, ColorTextureChain);
        ovr_CommitTextureSwapChain(Session, DepthTextureChain);
    }
};


OculusTextureBuffer eyeRenderTexture[2];

void SubmitStereoImageToHmd(ovrSession session, GLuint stereoTexture, int windowWidth, int windowHeight) {

    // Retrieve the texture swap chain index where we should write our eye image
    int currentIndex[2] = { 0, 0 };
    for (int eye = 0; eye < 2; ++eye) {
        ovr_GetTextureSwapChainCurrentIndex(session, eyeRenderTexture[eye]->ColorTextureChain, &currentIndex[eye]);
    }

    // For each eye
    for (int eye = 0; eye < 2; ++eye) {
        GLuint destTexId;
        ovr_GetTextureSwapChainBufferGL(session, eyeRenderTexture[eye]->ColorTextureChain, currentIndex[eye], &destTexId);

        // Set up the target framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, eyeRenderTexture[eye]->fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTexId, 0);

        // Set the viewport to the full size of the eye texture
        glViewport(0, 0, eyeRenderTexture[eye]->texSize.w, eyeRenderTexture[eye]->texSize.h);

        // Use the stereoTexture as the source for our blit
        glBindTexture(GL_TEXTURE_2D, stereoTexture);

        // Calculate the source texture coordinates for this eye
        int x = eye * windowWidth / 2;
        glBlitFramebuffer(
            x, 0, x + windowWidth / 2, windowHeight, // source rectangle
            0, 0, eyeRenderTexture[eye]->texSize.w, eyeRenderTexture[eye]->texSize.h, // destination rectangle
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        // Unbind the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Commit the swap chain texture
        ovr_CommitTextureSwapChain(session, eyeRenderTexture[eye]->ColorTextureChain);
    }

    // Now that the eye images are in their respective texture swap chains, you can submit them to the Oculus SDK
    ovrLayerEyeFov layer = {};
    layer.Header.Type = ovrLayerType_EyeFov;
    layer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft; // because OpenGL.

    // Populate the layer fields, assuming EyeRenderPose and other details are already computed elsewhere
    for (int eye = 0; eye < 2; ++eye) {
        layer.ColorTexture[eye] = eyeRenderTexture[eye]->ColorTextureChain;
        layer.Viewport[eye] = Recti(eyeRenderTexture[eye]->texSize);
        // Fill in other layer details like Fov and RenderPose
    }

    // Submit the frame to the Oculus SDK
    ovrLayerHeader* layers = &layer.Header;
    ovrResult result = ovr_SubmitFrame(session, 0, nullptr, &layers, 1);
    if (!OVR_SUCCESS(result)) {
        // Handle errors
    }
}

#endif

#if 0
void SubmitStereoImageToHmd(GLuint stereoTexture, int windowWidth, int windowHeight) {
    if (!hmd) {
        return; // Early exit if the HMD is not initialized
    }

    // Define the texture swap chain descriptor
    ovrTextureSwapChainDesc desc = {};
    desc.Type = ovrTexture_2D;
    desc.ArraySize = 1;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB; // Match this with your texture format
    desc.Width = windowWidth / 2; // Half the width for each eye
    desc.Height = windowHeight;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.StaticImage = ovrFalse;

    // Set up the texture swap chain for each eye
    ovrTextureSwapChain textureChain[2] = { 0 }; // This should be persistent between frames
    for (int eye = 0; eye < 2; ++eye) {
        ovrResult result = ovr_CreateTextureSwapChainGL(hmd, &desc, &textureChain[eye]);
        if (!OVR_SUCCESS(result)) {
            // Handle the error, creation of the texture swap chain failed
        }
    }

    // Get the current eye poses
    double sensorSampleTime = ovr_GetTimeInSeconds();

    ovrHmdDesc hmdDesc = ovr_GetHmdDesc(hmd);
    ovrEyeRenderDesc eyeRenderDesc[2];
    eyeRenderDesc[0] = ovr_GetRenderDesc(hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
    eyeRenderDesc[1] = ovr_GetRenderDesc(hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);
    ovrPosef EyeRenderPose[2];
    ovrPosef HmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
                                 eyeRenderDesc[1].HmdToEyePose };
    ovr_GetEyePoses(hmd, 0, true, HmdToEyePose, EyeRenderPose, &sensorSampleTime);

    ovrLayerEyeFov layer = {}; // This will describe our image layer
    layer.Header.Type = ovrLayerType_EyeFov;
    layer.Header.Flags = 0;

    // Bind the full stereo texture that contains both eyes' images side by side
    glBindTexture(GL_TEXTURE_2D, stereoTexture);

    // Submit the textures to the swap chain for each eye
    for (int eye = 0; eye < 2; ++eye) {
        int currentIndex = 0;
        ovr_GetTextureSwapChainCurrentIndex(hmd, textureChain[eye], &currentIndex);

        GLuint chainTexId;
        ovr_GetTextureSwapChainBufferGL(hmd, textureChain[eye], currentIndex, &chainTexId);

        glBindTexture(GL_TEXTURE_2D, chainTexId);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, eye * windowWidth, 0, windowWidth, windowHeight); // Copy the correct half of the stereo texture

        // Commit the texture to the swap chain
        ovr_CommitTextureSwapChain(hmd, textureChain[eye]);

        layer.ColorTexture[eye] = textureChain[eye];
        layer.Viewport[eye].Pos.x = 0;
        layer.Viewport[eye].Pos.y = 0;
        layer.Viewport[eye].Size = Recti(EyeRenderPose[eye]->GetSize());;
        layer.Fov[eye] = ovr_GetHmdDesc(hmd).DefaultEyeFov[eye];
        layer.RenderPose[eye] = EyeRenderPose[eye];
        layer.SensorSampleTime = sensorSampleTime;
    }

    ovrLayerHeader* layers = &layer.Header;
    ovr_SubmitFrame(hmd, 0, nullptr, &layers, 1);

    // Post submit, you might want to handle frame synchronization and any necessary clean-up
}

#endif