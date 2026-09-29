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
