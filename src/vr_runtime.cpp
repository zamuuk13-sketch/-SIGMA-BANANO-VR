#include "vr_runtime.h"

static BananoVRRuntimeInfo g_runtimeInfo{
    BananoVRRuntimeState::Ready,
    BananoVRBackend::SteamVRBridge,
    "Banano VR Runtime",
    "Base preparada para a ponte HMD + controles VR."
};

void BananoVRRuntimeInitialize() {
    // Etapa 10: somente a camada de arquitetura.
    // Nenhum driver real e nenhuma entrada de teclado/mouse sao usados aqui.
    g_runtimeInfo.state = BananoVRRuntimeState::Ready;
    g_runtimeInfo.backend = BananoVRBackend::SteamVRBridge;
}

const BananoVRRuntimeInfo& BananoVRRuntimeGetInfo() {
    return g_runtimeInfo;
}

const char* BananoVRRuntimeStateText(BananoVRRuntimeState state) {
    switch (state) {
    case BananoVRRuntimeState::Offline: return "Offline";
    case BananoVRRuntimeState::Preparing: return "Preparando";
    case BananoVRRuntimeState::Ready: return "Base pronta";
    }
    return "Desconhecido";
}

const char* BananoVRRuntimeBackendText(BananoVRBackend backend) {
    switch (backend) {
    case BananoVRBackend::None: return "Nenhum";
    case BananoVRBackend::SteamVRBridge: return "SteamVR / OpenXR";
    case BananoVRBackend::OpenXRRuntime: return "OpenXR Runtime";
    }
    return "Desconhecido";
}
