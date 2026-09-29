#include "banano_openxr_minimal.h"
#include <cstring>
#include <new>

struct BananoXrInstance {
    uint32_t magic;
    XrVersion apiVersion;
    BananoHmdPose pose;
    uint32_t viewCount;
};

static BananoHmdPose g_hmdPose{
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetHmdOrientation(
    float x, float y, float z, float w) {

    g_hmdPose.orientationX = x;
    g_hmdPose.orientationY = y;
    g_hmdPose.orientationZ = z;
    g_hmdPose.orientationW = w;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetHmdPosition(
    float x, float y, float z) {

    g_hmdPose.positionX = x;
    g_hmdPose.positionY = y;
    g_hmdPose.positionZ = z;
}

struct BananoControllerState {
    bool connected;
    BananoHmdPose pose;
};

static BananoControllerState g_leftController{
    false,
    { -0.20f, -0.10f, -0.45f, 0.0f, 0.0f, 0.0f, 1.0f }
};

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetLeftControllerPose(
    float x, float y, float z, float qx, float qy, float qz, float qw) {
    g_leftController.connected = true;
    g_leftController.pose.positionX = x;
    g_leftController.pose.positionY = y;
    g_leftController.pose.positionZ = z;
    g_leftController.pose.orientationX = qx;
    g_leftController.pose.orientationY = qy;
    g_leftController.pose.orientationZ = qz;
    g_leftController.pose.orientationW = qw;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetLeftControllerConnected(
    int connected) {
    g_leftController.connected = connected != 0;
}

static BananoControllerState g_rightController{
    false,
    { 0.20f, -0.10f, -0.45f, 0.0f, 0.0f, 0.0f, 1.0f }
};

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetRightControllerPose(
    float x, float y, float z, float qx, float qy, float qz, float qw) {
    g_rightController.connected = true;
    g_rightController.pose.positionX = x;
    g_rightController.pose.positionY = y;
    g_rightController.pose.positionZ = z;
    g_rightController.pose.orientationX = qx;
    g_rightController.pose.orientationY = qy;
    g_rightController.pose.orientationZ = qz;
    g_rightController.pose.orientationW = qw;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetRightControllerConnected(
    int connected) {
    g_rightController.connected = connected != 0;
}

static const uint32_t kBananoStereoViewCount = 2;
static const uint32_t kBananoRecommendedEyeWidth = 1024;
static const uint32_t kBananoRecommendedEyeHeight = 1024;

static XrResult BANANO_XR_CALL BananoCreateInstance(
    const XrInstanceCreateInfo* info,
    XrInstance* instance) {

    if (!instance || !info)
        return XR_ERROR_RUNTIME_FAILURE;

    *instance = XR_NULL_HANDLE;

    if (info->type != XR_TYPE_INSTANCE_CREATE_INFO ||
        info->createFlags != 0 ||
        info->applicationInfo.applicationName[0] == '\\0')
        return XR_ERROR_INITIALIZATION_FAILED;

    BananoXrInstance* object = new (std::nothrow) BananoXrInstance{};
    if (!object)
        return XR_ERROR_RUNTIME_FAILURE;

    object->magic = 0x42414E4F;
    object->apiVersion = info->applicationInfo.apiVersion;
    object->pose = g_hmdPose;
    object->viewCount = kBananoStereoViewCount;

    *instance = reinterpret_cast<XrInstance>(object);
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoGetInstanceProcAddr(
    XrInstance,
    const char* name,
    PFN_xrVoidFunction* function) {

    if (!function || !name)
        return XR_ERROR_RUNTIME_FAILURE;

    *function = nullptr;

    if (strcmp(name, "xrGetInstanceProcAddr") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoGetInstanceProcAddr);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrCreateInstance") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoCreateInstance);
        return XR_SUCCESS;
    }

    return XR_ERROR_FUNCTION_UNSUPPORTED;
}

extern "C" BANANO_EXPORT XrResult BANANO_XR_CALL xrNegotiateLoaderRuntimeInterface(
    const XrNegotiateLoaderInfo* loaderInfo,
    XrNegotiateRuntimeRequest* runtimeRequest) {

    if (!loaderInfo || !runtimeRequest)
        return XR_ERROR_RUNTIME_FAILURE;

    if (loaderInfo->structType != XR_LOADER_INTERFACE_STRUCT_LOADER_INFO ||
        loaderInfo->structVersion != XR_LOADER_INFO_STRUCT_VERSION ||
        loaderInfo->structSize < sizeof(XrNegotiateLoaderInfo))
        return XR_ERROR_RUNTIME_FAILURE;

    if (runtimeRequest->structType != XR_LOADER_INTERFACE_STRUCT_RUNTIME_REQUEST ||
        runtimeRequest->structVersion != XR_RUNTIME_INFO_STRUCT_VERSION ||
        runtimeRequest->structSize < sizeof(XrNegotiateRuntimeRequest))
        return XR_ERROR_RUNTIME_FAILURE;

    if (loaderInfo->minInterfaceVersion > XR_CURRENT_LOADER_RUNTIME_VERSION ||
        loaderInfo->maxInterfaceVersion < XR_CURRENT_LOADER_RUNTIME_VERSION)
        return XR_ERROR_RUNTIME_FAILURE;

    runtimeRequest->runtimeInterfaceVersion = XR_CURRENT_LOADER_RUNTIME_VERSION;
    runtimeRequest->runtimeApiVersion = loaderInfo->minApiVersion;
    runtimeRequest->getInstanceProcAddr = BananoGetInstanceProcAddr;

    return XR_SUCCESS;
}
