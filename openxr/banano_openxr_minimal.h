#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
#define BANANO_XR_CALL __stdcall
#define BANANO_EXPORT __declspec(dllexport)
#else
#define BANANO_XR_CALL
#define BANANO_EXPORT
#endif

typedef int32_t XrResult;
typedef uint64_t XrVersion;
typedef struct XrInstance_T* XrInstance;
typedef void (BANANO_XR_CALL *PFN_xrVoidFunction)(void);
typedef XrResult (BANANO_XR_CALL *PFN_xrGetInstanceProcAddr)(
    XrInstance instance, const char* name, PFN_xrVoidFunction* function);

enum {
    XR_SUCCESS = 0,
    XR_ERROR_FUNCTION_UNSUPPORTED = -7,
    XR_ERROR_INITIALIZATION_FAILED = -6,
    XR_ERROR_RUNTIME_FAILURE = -2
};

enum {
    XR_LOADER_INTERFACE_STRUCT_LOADER_INFO = 1,
    XR_LOADER_INTERFACE_STRUCT_RUNTIME_REQUEST = 3
};

#define XR_LOADER_INFO_STRUCT_VERSION 1
#define XR_RUNTIME_INFO_STRUCT_VERSION 1
#define XR_CURRENT_LOADER_RUNTIME_VERSION 1

typedef struct XrNegotiateLoaderInfo {
    int32_t structType;
    uint32_t structVersion;
    size_t structSize;
    uint32_t minInterfaceVersion;
    uint32_t maxInterfaceVersion;
    XrVersion minApiVersion;
    XrVersion maxApiVersion;
} XrNegotiateLoaderInfo;

typedef struct XrNegotiateRuntimeRequest {
    int32_t structType;
    uint32_t structVersion;
    size_t structSize;
    uint32_t runtimeInterfaceVersion;
    XrVersion runtimeApiVersion;
    PFN_xrGetInstanceProcAddr getInstanceProcAddr;
} XrNegotiateRuntimeRequest;

typedef struct XrInstanceCreateInfo XrInstanceCreateInfo;
typedef XrResult (BANANO_XR_CALL *PFN_xrCreateInstance)(
    const XrInstanceCreateInfo* info, XrInstance* instance);

#define XR_NULL_HANDLE nullptr

typedef int64_t XrFlags64;
typedef int32_t XrStructureType;
typedef XrFlags64 XrInstanceCreateFlags;

#define XR_TYPE_INSTANCE_CREATE_INFO 3
#define XR_MAX_APPLICATION_NAME_SIZE 128
#define XR_MAX_ENGINE_NAME_SIZE 128
#define XR_MAX_API_LAYER_NAME_SIZE 256
#define XR_MAX_EXTENSION_NAME_SIZE 128

typedef struct XrApplicationInfo {
    char applicationName[XR_MAX_APPLICATION_NAME_SIZE];
    uint32_t applicationVersion;
    char engineName[XR_MAX_ENGINE_NAME_SIZE];
    uint32_t engineVersion;
    XrVersion apiVersion;
} XrApplicationInfo;

typedef struct XrInstanceCreateInfo {
    XrStructureType type;
    const void* next;
    XrInstanceCreateFlags createFlags;
    XrApplicationInfo applicationInfo;
    uint32_t enabledApiLayerCount;
    const char* const* enabledApiLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* enabledExtensionNames;
} XrInstanceCreateInfo;

typedef struct BananoHmdPose {
    float positionX;
    float positionY;
    float positionZ;
    float orientationX;
    float orientationY;
    float orientationZ;
    float orientationW;
} BananoHmdPose;

typedef uint32_t XrViewConfigurationType;
typedef struct XrViewConfigurationView {
    XrStructureType type;
    const void* next;
    uint32_t recommendedImageRectWidth;
    uint32_t maxImageRectWidth;
    uint32_t recommendedImageRectHeight;
    uint32_t maxImageRectHeight;
    uint32_t recommendedSwapchainSampleCount;
    uint32_t maxSwapchainSampleCount;
} XrViewConfigurationView;

#define XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO 2
#define XR_TYPE_VIEW_CONFIGURATION_VIEW 41

typedef struct BananoControllerPose {
    float positionX;
    float positionY;
    float positionZ;
    float orientationX;
    float orientationY;
    float orientationZ;
    float orientationW;
    int connected;
} BananoControllerPose;

#define BANANO_CONTROLLER_LEFT 0
#define BANANO_CONTROLLER_RIGHT 1

#define BANANO_FACE_BUTTON_A 0
#define BANANO_FACE_BUTTON_B 1
#define BANANO_FACE_BUTTON_X 2
#define BANANO_FACE_BUTTON_Y 3

#define BANANO_ANALOG_MIN 0.0f
#define BANANO_ANALOG_MAX 1.0f

#define BANANO_THUMBSTICK_MIN -1.0f
#define BANANO_THUMBSTICK_MAX 1.0f

typedef struct BananoVRInputSnapshot {
    uint64_t sequence;
    int activeProfile;
    int leftA;
    int leftB;
    int leftX;
    int leftY;
    int rightA;
    int rightB;
    int rightX;
    int rightY;
    float leftTrigger;
    float rightTrigger;
    float leftGrip;
    float rightGrip;
    float leftThumbstickX;
    float leftThumbstickY;
    float rightThumbstickX;
    float rightThumbstickY;
    int leftThumbstickClick;
    int rightThumbstickClick;
} BananoVRInputSnapshot;

#define BANANO_INPUT_SNAPSHOT_VERSION 1

typedef uint64_t XrSystemId;
typedef struct XrSystemGetInfo {
    XrStructureType type;
    const void* next;
    uint32_t formFactor;
} XrSystemGetInfo;

#define XR_TYPE_SYSTEM_GET_INFO 4
#define XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY 1
#define BANANO_XR_SYSTEM_ID 1

typedef uint32_t XrBool32;

#define XR_TYPE_SYSTEM_PROPERTIES 39
#define XR_MAX_SYSTEM_NAME_SIZE 256
#define XR_MIN_COMPOSITION_LAYERS_SUPPORTED 16

typedef struct XrSystemGraphicsProperties {
    uint32_t maxSwapchainImageHeight;
    uint32_t maxSwapchainImageWidth;
    uint32_t maxLayerCount;
} XrSystemGraphicsProperties;

typedef struct XrSystemTrackingProperties {
    XrBool32 orientationTracking;
    XrBool32 positionTracking;
} XrSystemTrackingProperties;

typedef struct XrSystemProperties {
    XrStructureType type;
    void* next;
    XrSystemId systemId;
    uint32_t vendorId;
    char systemName[XR_MAX_SYSTEM_NAME_SIZE];
    XrSystemGraphicsProperties graphicsProperties;
    XrSystemTrackingProperties trackingProperties;
} XrSystemProperties;

#define XR_ERROR_SIZE_INSUFFICIENT -13
#define XR_ERROR_SESSION_NOT_RUNNING -18

#define XR_ERROR_SYSTEM_INVALID -17
#define XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED -29
#define XR_TYPE_VIEW_CONFIGURATION_PROPERTIES 48

typedef struct XrViewConfigurationProperties {
    XrStructureType type;
    void* next;
    XrViewConfigurationType viewConfigurationType;
    XrBool32 fovMutable;
} XrViewConfigurationProperties;

typedef struct XrSession_T* XrSession;
typedef uint64_t XrSessionCreateFlags;

#define XR_TYPE_SESSION_CREATE_INFO 8

typedef struct XrSessionCreateInfo {
    XrStructureType type;
    const void* next;
    XrSessionCreateFlags createFlags;
    XrSystemId systemId;
} XrSessionCreateInfo;

typedef uint32_t XrSessionState;

#define XR_TYPE_SESSION_BEGIN_INFO 10
#define XR_SESSION_STATE_IDLE 1
#define XR_SESSION_STATE_READY 2
#define XR_SESSION_STATE_SYNCHRONIZED 3
#define XR_SESSION_STATE_VISIBLE 4
#define XR_SESSION_STATE_FOCUSED 5
#define XR_SESSION_STATE_STOPPING 6
#define XR_SESSION_STATE_EXITING 8

typedef struct XrSessionBeginInfo {
    XrStructureType type;
    const void* next;
    XrViewConfigurationType primaryViewConfigurationType;
} XrSessionBeginInfo;

typedef int64_t XrTime;
typedef int64_t XrDuration;

#define XR_TYPE_FRAME_WAIT_INFO 33
#define XR_TYPE_FRAME_STATE 35
#define XR_TYPE_FRAME_BEGIN_INFO 15

typedef struct XrFrameWaitInfo {
    XrStructureType type;
    const void* next;
} XrFrameWaitInfo;

typedef struct XrFrameState {
    XrStructureType type;
    void* next;
    XrDuration predictedDisplayPeriod;
    XrBool32 shouldRender;
    XrTime predictedDisplayTime;
} XrFrameState;

typedef struct XrFrameBeginInfo {
    XrStructureType type;
    const void* next;
} XrFrameBeginInfo;

typedef uint32_t XrEnvironmentBlendMode;
typedef XrFlags64 XrCompositionLayerFlags;

#define XR_TYPE_FRAME_END_INFO 16
#define XR_ENVIRONMENT_BLEND_MODE_OPAQUE 1
#define XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT 2

typedef struct XrFrameEndInfo {
    XrStructureType type;
    const void* next;
    XrTime displayTime;
    XrEnvironmentBlendMode environmentBlendMode;
    uint32_t layerCount;
    const void* const* layers;
} XrFrameEndInfo;

typedef struct XrSwapchain_T* XrSwapchain;
typedef uint32_t XrSwapchainUsageFlags;
typedef XrFlags64 XrSwapchainCreateFlags;

#define XR_TYPE_SWAPCHAIN_CREATE_INFO 9
#define XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT 0x00000010
#define XR_SWAPCHAIN_USAGE_SAMPLED_BIT 0x00000001
#define XR_SWAPCHAIN_CREATE_STATIC_IMAGE_BIT 0x00000002
#define XR_SWAPCHAIN_FORMAT_R8G8B8A8 43

typedef struct XrSwapchainCreateInfo {
    XrStructureType type;
    const void* next;
    XrSwapchainCreateFlags createFlags;
    XrSwapchainUsageFlags usageFlags;
    int64_t format;
    uint32_t sampleCount;
    uint32_t width;
    uint32_t height;
    uint32_t faceCount;
    uint32_t arraySize;
    uint32_t mipCount;
} XrSwapchainCreateInfo;

typedef struct XrSwapchain_T* XrSwapchain;
typedef XrFlags64 XrSwapchainCreateFlags;
typedef XrFlags64 XrSwapchainUsageFlags;

#define XR_TYPE_SWAPCHAIN_CREATE_INFO 9
#define XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT 0x00000001
#define XR_SWAPCHAIN_USAGE_SAMPLED_BIT 0x00000020

typedef struct XrSwapchainCreateInfo {
    XrStructureType type;
    const void* next;
    XrSwapchainCreateFlags createFlags;
    XrSwapchainUsageFlags usageFlags;
    int64_t format;
    uint32_t sampleCount;
    uint32_t width;
    uint32_t height;
    uint32_t faceCount;
    uint32_t arraySize;
    uint32_t mipCount;
} XrSwapchainCreateInfo;

#define XR_ERROR_FEATURE_UNSUPPORTED -8
#define XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED -45

typedef uint32_t XrSwapchainImageAcquireInfoDummy;

#define XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO 7
#define XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO 48
#define XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO 10

typedef struct XrSwapchainImageAcquireInfo {
    XrStructureType type;
    const void* next;
} XrSwapchainImageAcquireInfo;

typedef struct XrSwapchainImageWaitInfo {
    XrStructureType type;
    const void* next;
    XrDuration timeout;
} XrSwapchainImageWaitInfo;

typedef struct XrSwapchainImageReleaseInfo {
    XrStructureType type;
    const void* next;
} XrSwapchainImageReleaseInfo;

typedef struct XrSwapchainImageBaseHeader {
    XrStructureType type;
    void* next;
} XrSwapchainImageBaseHeader;

typedef struct XrSwapchainImageDummy {
    XrStructureType type;
    void* next;
    uint32_t imageIndex;
} XrSwapchainImageDummy;

typedef struct XrSwapchainImageBaseHeader {
    XrStructureType type;
    void* next;
} XrSwapchainImageBaseHeader;

typedef struct XrSwapchainImageAcquireInfo {
    XrStructureType type;
    const void* next;
} XrSwapchainImageAcquireInfo;

typedef struct XrSwapchainImageWaitInfo {
    XrStructureType type;
    const void* next;
    XrDuration timeout;
} XrSwapchainImageWaitInfo;

typedef struct XrSwapchainImageReleaseInfo {
    XrStructureType type;
    const void* next;
} XrSwapchainImageReleaseInfo;

#define XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO 100
#define XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO 101
#define XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO 102
#define XR_TYPE_SWAPCHAIN_IMAGE_BASE_HEADER 0
#define XR_ERROR_CALL_ORDER_INVALID -37

#define XR_TYPE_REFERENCE_SPACE_CREATE_INFO 37
#define XR_TYPE_SPACE_LOCATION 106

#define XR_REFERENCE_SPACE_TYPE_VIEW 1
#define XR_REFERENCE_SPACE_TYPE_LOCAL 2

typedef struct XrSpace_T* XrSpace;
typedef XrFlags64 XrSpaceLocationFlags;

#define XR_SPACE_LOCATION_ORIENTATION_VALID_BIT 0x00000001
#define XR_SPACE_LOCATION_POSITION_VALID_BIT 0x00000002

typedef struct XrPosef {
    struct { float x, y, z, w; } orientation;
    struct { float x, y, z; } position;
} XrPosef;

typedef struct XrReferenceSpaceCreateInfo {
    XrStructureType type;
    const void* next;
    int32_t referenceSpaceType;
    XrPosef poseInReferenceSpace;
} XrReferenceSpaceCreateInfo;

typedef struct XrSpaceLocation {
    XrStructureType type;
    void* next;
    XrSpaceLocationFlags locationFlags;
    XrPosef pose;
} XrSpaceLocation;
