#include "../openxr/banano_openxr_minimal.h"
#include <windows.h>
#include <cstdio>
#include <cstring>

using PFN_Negotiate = XrResult (BANANO_XR_CALL*)(
    const XrNegotiateLoaderInfo*, XrNegotiateRuntimeRequest*);

static bool Check(XrResult result, const char* step) {
    if (result != XR_SUCCESS) {
        std::printf("[FAIL] %s -> %d\n", step, result);
        return false;
    }
    std::printf("[OK] %s\n", step);
    return true;
}

int main() {
    HMODULE runtime = LoadLibraryA("BananoVRRuntime.dll");
    if (!runtime) {
        std::printf("[FAIL] BananoVRRuntime.dll nao encontrado.\n");
        return 1;
    }

    auto negotiate = reinterpret_cast<PFN_Negotiate>(
        GetProcAddress(runtime, "xrNegotiateLoaderRuntimeInterface"));
    if (!negotiate) {
        std::printf("[FAIL] xrNegotiateLoaderRuntimeInterface nao exportado.\n");
        FreeLibrary(runtime);
        return 1;
    }

    XrNegotiateLoaderInfo loaderInfo{};
    loaderInfo.structType = XR_LOADER_INTERFACE_STRUCT_LOADER_INFO;
    loaderInfo.structVersion = XR_LOADER_INFO_STRUCT_VERSION;
    loaderInfo.structSize = sizeof(loaderInfo);
    loaderInfo.minInterfaceVersion = XR_CURRENT_LOADER_RUNTIME_VERSION;
    loaderInfo.maxInterfaceVersion = XR_CURRENT_LOADER_RUNTIME_VERSION;
    loaderInfo.minApiVersion = 1;
    loaderInfo.maxApiVersion = 1;

    XrNegotiateRuntimeRequest request{};
    request.structType = XR_LOADER_INTERFACE_STRUCT_RUNTIME_REQUEST;
    request.structVersion = XR_RUNTIME_INFO_STRUCT_VERSION;
    request.structSize = sizeof(request);

    if (!Check(negotiate(&loaderInfo, &request), "Negociacao Loader/Runtime"))
        return 1;

    if (!request.getInstanceProcAddr) {
        std::printf("[FAIL] getInstanceProcAddr ausente.\n");
        FreeLibrary(runtime);
        return 1;
    }

    auto getProc = request.getInstanceProcAddr;

    auto get = [&](const char* name) -> PFN_xrVoidFunction {
        PFN_xrVoidFunction fn = nullptr;
        if (getProc(nullptr, name, &fn) != XR_SUCCESS) return nullptr;
        return fn;
    };

    auto createInstance = reinterpret_cast<PFN_xrCreateInstance>(
        get(nullptr, "xrCreateInstance"));
    if (!createInstance) {
        std::printf("[FAIL] xrCreateInstance ausente.\n");
        FreeLibrary(runtime);
        return 1;
    }

    XrInstanceCreateInfo info{};
    info.type = XR_TYPE_INSTANCE_CREATE_INFO;
    std::strncpy(info.applicationInfo.applicationName, "Banano Smoke Test",
                 XR_MAX_APPLICATION_NAME_SIZE - 1);
    info.applicationInfo.apiVersion = 1;

    XrInstance instance = XR_NULL_HANDLE;
    if (!Check(createInstance(&info, &instance), "Criacao de instancia"))
        return 1;

    auto getSystem = reinterpret_cast<XrResult (BANANO_XR_CALL*)(
        XrInstance, const XrSystemGetInfo*, XrSystemId*)>(
        get("xrGetSystem"));

    XrSystemGetInfo systemInfo{};
    systemInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId systemId = 0;

    if (!getSystem ||
        !Check(getSystem(instance, &systemInfo, &systemId), "Deteccao do sistema Banano VR"))
        return 1;

    std::printf("[OK] SystemId: %llu\n",
        static_cast<unsigned long long>(systemId));
    std::printf("[BANANO VR] Smoke test concluido.\n");

    FreeLibrary(runtime);
    return 0;
}
