#include "banano_openxr_minimal.h"
#include <cstring>

static XrResult BANANO_XR_CALL BananoCreateInstance(
    const XrInstanceCreateInfo*,
    XrInstance* instance) {

    if (instance) *instance = XR_NULL_HANDLE;

    // Etapa 14: o caminho de criacao ja esta exposto ao Loader,
    // mas a instancia real ainda depende das proximas etapas do runtime.
    return XR_ERROR_INITIALIZATION_FAILED;
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
