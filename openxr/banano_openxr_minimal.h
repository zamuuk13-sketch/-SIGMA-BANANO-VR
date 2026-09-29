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
