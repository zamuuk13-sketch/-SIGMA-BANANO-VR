#pragma once

enum class BananoVRRuntimeState {
    Offline,
    Preparing,
    Ready
};

enum class BananoVRBackend {
    None,
    SteamVRBridge,
    OpenXRRuntime
};

struct BananoVRRuntimeInfo {
    BananoVRRuntimeState state;
    BananoVRBackend backend;
    const char* name;
    const char* description;
};

void BananoVRRuntimeInitialize();
const BananoVRRuntimeInfo& BananoVRRuntimeGetInfo();
const char* BananoVRRuntimeStateText(BananoVRRuntimeState state);
const char* BananoVRRuntimeBackendText(BananoVRBackend backend);
