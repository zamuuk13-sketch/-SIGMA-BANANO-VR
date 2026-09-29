#include "banano_openxr_minimal.h"
#include <cstring>
#include <new>\n#include <chrono>
#include <cmath>

struct BananoXrSession {
    uint32_t magic;
    XrInstance instance;
    XrSystemId systemId;
    XrViewConfigurationType viewConfigurationType;
    bool running;
};

static XrTime BananoNowNs() {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(
        steady_clock::now().time_since_epoch()).count();
}

struct BananoXrSwapchain {
    uint32_t magic;
    XrSession session;
    uint32_t width;
    uint32_t height;
    int64_t format;
    uint32_t imageCount;
};

static XrResult BANANO_XR_CALL BananoCreateSwapchain(
    XrSession session,
    const XrSwapchainCreateInfo* info,
    XrSwapchain* swapchain) {

    if (!session || !info || !swapchain)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* sessionObject =
        reinterpret_cast<BananoXrSession*>(session);
    if (!sessionObject->running)
        return XR_ERROR_SESSION_NOT_RUNNING;

    if (info->type != XR_TYPE_SWAPCHAIN_CREATE_INFO ||
        info->width == 0 || info->height == 0 ||
        info->faceCount != 1 || info->arraySize == 0 ||
        info->mipCount != 1 || info->sampleCount == 0)
        return XR_ERROR_RUNTIME_FAILURE;

    if (info->format != XR_SWAPCHAIN_FORMAT_R8G8B8A8)
        return XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED;

    BananoXrSwapchain* object = new (std::nothrow) BananoXrSwapchain{};
    if (!object)
        return XR_ERROR_RUNTIME_FAILURE;

    object->magic = 0x53574150;
    object->session = session;
    object->width = info->width;
    object->height = info->height;
    object->format = info->format;
    object->imageCount = 3;

    *swapchain = reinterpret_cast<XrSwapchain>(object);
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoEnumerateSwapchainImages(
    XrSwapchain swapchain,
    uint32_t capacityInput,
    uint32_t* countOutput,
    void* images) {

    if (!swapchain || !countOutput)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSwapchain* object =
        reinterpret_cast<BananoXrSwapchain*>(swapchain);

    *countOutput = object->imageCount;
    if (capacityInput == 0 || !images)
        return XR_SUCCESS;

    if (capacityInput < object->imageCount)
        return XR_ERROR_SIZE_INSUFFICIENT;

    // As imagens GPU reais serão ligadas quando a API gráfica da sessão
    // for implementada. Nesta etapa o runtime expõe apenas a contagem.
    return XR_SUCCESS;
}

struct BananoXrSwapchain {
    uint32_t magic;
    XrSession session;
    int64_t format;
    uint32_t width;
    uint32_t height;
    uint32_t arraySize;
    uint32_t imageCount;
    uint32_t acquiredImage;
    bool imageAcquired;
    bool imageReady;
    bool imageReleased;
};

static XrResult BANANO_XR_CALL BananoCreateSwapchain(
    XrSession session,
    const XrSwapchainCreateInfo* createInfo,
    XrSwapchain* swapchain) {

    if (!session || !createInfo || !swapchain)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* sessionObject =
        reinterpret_cast<BananoXrSession*>(session);
    if (!sessionObject->running)
        return XR_ERROR_SESSION_NOT_RUNNING;

    if (createInfo->type != XR_TYPE_SWAPCHAIN_CREATE_INFO)
        return XR_ERROR_RUNTIME_FAILURE;

    if (createInfo->createFlags != 0 ||
        (createInfo->usageFlags &
            ~(XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT |
              XR_SWAPCHAIN_USAGE_SAMPLED_BIT)) != 0 ||
        createInfo->format == 0 ||
        createInfo->sampleCount != 1 ||
        createInfo->width == 0 ||
        createInfo->height == 0 ||
        createInfo->faceCount != 1 ||
        createInfo->arraySize == 0 ||
        createInfo->mipCount != 1)
        return XR_ERROR_FEATURE_UNSUPPORTED;

    if (createInfo->width > kBananoRecommendedEyeWidth ||
        createInfo->height > kBananoRecommendedEyeHeight)
        return XR_ERROR_FEATURE_UNSUPPORTED;

    BananoXrSwapchain* object = new (std::nothrow) BananoXrSwapchain{};
    if (!object)
        return XR_ERROR_RUNTIME_FAILURE;

    object->magic = 0x53574348;
    object->session = session;
    object->format = createInfo->format;
    object->width = createInfo->width;
    object->height = createInfo->height;
    object->arraySize = createInfo->arraySize;
    object->imageCount = 2;
    object->acquiredImage = 0;
    object->imageAcquired = false;
    object->imageReady = false;
    object->imageReleased = true;

    *swapchain = reinterpret_cast<XrSwapchain>(object);
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoEnumerateSwapchainImages(
    XrSwapchain swapchain,
    uint32_t capacityInput,
    uint32_t* countOutput,
    XrSwapchainImageBaseHeader* images) {

    if (!swapchain || !countOutput)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSwapchain* object =
        reinterpret_cast<BananoXrSwapchain*>(swapchain);

    *countOutput = object->imageCount;
    if (capacityInput == 0 || !images)
        return XR_SUCCESS;

    if (capacityInput < object->imageCount)
        return XR_ERROR_SIZE_INSUFFICIENT;

    for (uint32_t i = 0; i < object->imageCount; ++i) {
        if (images[i].type == 0)
            images[i].type = XR_TYPE_SWAPCHAIN_IMAGE_DUMMY;
        images[i].next = nullptr;
    }

    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoAcquireSwapchainImage(
    XrSwapchain swapchain,
    const XrSwapchainImageAcquireInfo* acquireInfo,
    uint32_t* index) {

    if (!swapchain || !acquireInfo || !index)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSwapchain* object =
        reinterpret_cast<BananoXrSwapchain*>(swapchain);

    if (acquireInfo->type != XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO)
        return XR_ERROR_RUNTIME_FAILURE;

    if (object->imageAcquired)
        return XR_ERROR_RUNTIME_FAILURE;

    object->acquiredImage =
        (object->acquiredImage + 1) % object->imageCount;
    object->imageAcquired = true;
    object->imageReady = false;
    object->imageReleased = false;
    *index = object->acquiredImage;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoWaitSwapchainImage(
    XrSwapchain swapchain,
    const XrSwapchainImageWaitInfo* waitInfo) {

    if (!swapchain || !waitInfo)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSwapchain* object =
        reinterpret_cast<BananoXrSwapchain*>(swapchain);

    if (waitInfo->type != XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO)
        return XR_ERROR_RUNTIME_FAILURE;

    if (!object->imageAcquired)
        return XR_ERROR_RUNTIME_FAILURE;

    object->imageReady = true;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoReleaseSwapchainImage(
    XrSwapchain swapchain,
    const XrSwapchainImageReleaseInfo* releaseInfo) {

    if (!swapchain || !releaseInfo)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSwapchain* object =
        reinterpret_cast<BananoXrSwapchain*>(swapchain);

    if (releaseInfo->type != XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO)
        return XR_ERROR_RUNTIME_FAILURE;

    if (!object->imageAcquired || !object->imageReady)
        return XR_ERROR_RUNTIME_FAILURE;

    object->imageAcquired = false;
    object->imageReady = false;
    object->imageReleased = true;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoEndFrame(
    XrSession session,
    const XrFrameEndInfo* frameEndInfo) {

    if (!session || !frameEndInfo)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* object = reinterpret_cast<BananoXrSession*>(session);
    if (!object->running)
        return XR_ERROR_SESSION_NOT_RUNNING;

    if (frameEndInfo->type != XR_TYPE_FRAME_END_INFO)
        return XR_ERROR_RUNTIME_FAILURE;

    if (frameEndInfo->environmentBlendMode != XR_ENVIRONMENT_BLEND_MODE_OPAQUE)
        return XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED;

    if (frameEndInfo->layerCount > 0 && !frameEndInfo->layers)
        return XR_ERROR_RUNTIME_FAILURE;

    // Etapa 37: aceita o fechamento do frame e a lista de layers.
    // A composicao grafica real sera ligada ao swapchain nas proximas etapas.
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoWaitFrame(
    XrSession session,
    const XrFrameWaitInfo*,
    XrFrameState* frameState) {

    if (!session || !frameState)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* object = reinterpret_cast<BananoXrSession*>(session);
    if (!object->running)
        return XR_ERROR_SESSION_NOT_RUNNING;

    if (frameState->type != XR_TYPE_FRAME_STATE)
        return XR_ERROR_RUNTIME_FAILURE;

    frameState->predictedDisplayPeriod = 16666666;
    frameState->predictedDisplayTime = BananoNowNs() + 16666666;
    frameState->shouldRender = 1;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoBeginFrame(
    XrSession session,
    const XrFrameBeginInfo* frameBeginInfo) {

    if (!session || !frameBeginInfo)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* object = reinterpret_cast<BananoXrSession*>(session);
    if (!object->running)
        return XR_ERROR_SESSION_NOT_RUNNING;

    if (frameBeginInfo->type != XR_TYPE_FRAME_BEGIN_INFO)
        return XR_ERROR_RUNTIME_FAILURE;

    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoBeginSession(
    XrSession session,
    const XrSessionBeginInfo* beginInfo) {

    if (!session || !beginInfo)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* object = reinterpret_cast<BananoXrSession*>(session);

    if (beginInfo->type != XR_TYPE_SESSION_BEGIN_INFO ||
        beginInfo->primaryViewConfigurationType !=
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO)
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;

    if (object->running)
        return XR_ERROR_RUNTIME_FAILURE;

    object->running = true;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoCreateSession(
    XrInstance instance,
    const XrSessionCreateInfo* info,
    XrSession* session) {

    if (!instance || !info || !session)
        return XR_ERROR_RUNTIME_FAILURE;

    if (info->type != XR_TYPE_SESSION_CREATE_INFO ||
        info->createFlags != 0 ||
        info->systemId != BANANO_XR_SYSTEM_ID)
        return XR_ERROR_RUNTIME_FAILURE;

    BananoXrSession* object = new (std::nothrow) BananoXrSession{};
    if (!object)
        return XR_ERROR_RUNTIME_FAILURE;

    object->magic = 0x53455353;
    object->instance = instance;
    object->systemId = info->systemId;
    object->viewConfigurationType =
        XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    object->running = false;

    *session = reinterpret_cast<XrSession>(object);
    return XR_SUCCESS;
}

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

struct BananoInputProfile {
    char name[64];
    bool enabled;
};

static constexpr int BANANO_MAX_INPUT_PROFILES = 8;
static BananoInputProfile g_inputProfiles[BANANO_MAX_INPUT_PROFILES]{};
static int g_inputProfileCount = 0;
static int g_activeInputProfile = -1;

extern "C" BANANO_EXPORT int BANANO_XR_CALL BananoVRRuntimeCreateInputProfile(
    const char* name) {
    if (!name || !name[0] || g_inputProfileCount >= BANANO_MAX_INPUT_PROFILES)
        return -1;

    for (int i = 0; i < g_inputProfileCount; ++i) {
        if (strcmp(g_inputProfiles[i].name, name) == 0)
            return i;
    }

    std::strncpy(g_inputProfiles[g_inputProfileCount].name,
        name, sizeof(g_inputProfiles[g_inputProfileCount].name) - 1);
    g_inputProfiles[g_inputProfileCount].name[
        sizeof(g_inputProfiles[g_inputProfileCount].name) - 1] = '\\0';
    g_inputProfiles[g_inputProfileCount].enabled = true;
    return g_inputProfileCount++;
}

extern "C" BANANO_EXPORT int BANANO_XR_CALL BananoVRRuntimeSetActiveInputProfile(
    int profileId) {
    if (profileId < 0 || profileId >= g_inputProfileCount ||
        !g_inputProfiles[profileId].enabled)
        return 0;
    g_activeInputProfile = profileId;
    return 1;
}

extern "C" BANANO_EXPORT int BANANO_XR_CALL BananoVRRuntimeGetActiveInputProfile() {
    return g_activeInputProfile;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetInputProfileEnabled(
    int profileId, int enabled) {
    if (profileId < 0 || profileId >= g_inputProfileCount)
        return;
    g_inputProfiles[profileId].enabled = enabled != 0;
    if (!g_inputProfiles[profileId].enabled && g_activeInputProfile == profileId)
        g_activeInputProfile = -1;
}

enum class BananoInputKind {
    FaceButton,
    Trigger,
    Grip,
    Thumbstick,
    ThumbstickClick,
    ControllerPose,
    HmdPose
};

struct BananoInputEvent {
    int controller;
    BananoInputKind kind;
    int control;
    float valueX;
    float valueY;
    float value;
    bool pressed;
};

static uint64_t g_inputSequence = 0;

extern "C" BANANO_EXPORT uint64_t BANANO_XR_CALL BananoVRRuntimeGetInputSequence() {
    return g_inputSequence;
}

static BananoInputEvent g_lastInputEvent{
    -1,
    BananoInputKind::FaceButton,
    -1,
    0.0f,
    0.0f,
    0.0f,
    false
};

enum class BananoMarkerControl {
    None = -1,
    LeftA = 0,
    LeftB = 1,
    LeftX = 2,
    LeftY = 3,
    LeftTrigger = 4,
    LeftGrip = 5,
    LeftThumbstick = 6,
    LeftThumbstickClick = 7,
    RightA = 8,
    RightB = 9,
    RightX = 10,
    RightY = 11,
    RightTrigger = 12,
    RightGrip = 13,
    RightThumbstick = 14,
    RightThumbstickClick = 15
};

struct BananoMarkerBinding {
    int markerId;
    BananoMarkerControl control;
};

static constexpr int BANANO_MAX_MARKER_BINDINGS = 64;
static BananoMarkerBinding g_markerBindings[BANANO_MAX_MARKER_BINDINGS]{};
static int g_markerBindingCount = 0;

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeClearMarkerBindings() {
    g_markerBindingCount = 0;
}

extern "C" BANANO_EXPORT int BANANO_XR_CALL BananoVRRuntimeBindMarker(
    int markerId, int control) {
    if (markerId <= 0 ||
        control < static_cast<int>(BananoMarkerControl::LeftA) ||
        control > static_cast<int>(BananoMarkerControl::RightThumbstickClick)) {
        return 0;
    }

    for (int i = 0; i < g_markerBindingCount; ++i) {
        if (g_markerBindings[i].markerId == markerId ||
            static_cast<int>(g_markerBindings[i].control) == control) {
            return 0;
        }
    }

    if (g_markerBindingCount >= BANANO_MAX_MARKER_BINDINGS)
        return 0;

    g_markerBindings[g_markerBindingCount++] = {
        markerId,
        static_cast<BananoMarkerControl>(control)
    };
    return 1;
}

static BananoMarkerControl BananoFindMarkerControl(int markerId) {
    for (int i = 0; i < g_markerBindingCount; ++i) {
        if (g_markerBindings[i].markerId == markerId)
            return g_markerBindings[i].control;
    }
    return BananoMarkerControl::None;
}

extern "C" BANANO_EXPORT int BANANO_XR_CALL BananoVRRuntimeGetMarkerControl(int markerId) {
    return static_cast<int>(BananoFindMarkerControl(markerId));
}

static BananoMarkerControl BananoFindMarkerControl(int markerId);
extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetThumbstick(int controller, float x, float y);
extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetThumbstickClick(int controller, int pressed);
extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetTrigger(int controller, float value);
extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetGrip(int controller, float value);

static float BananoClampUnit(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

static float BananoClampSigned(float value) {
    return value < -1.0f ? -1.0f : (value > 1.0f ? 1.0f : value);
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetMarkerHandPosition(
    int markerId, float handX, float handY, int touching) {
    const BananoMarkerControl control = BananoFindMarkerControl(markerId);
    const bool pressed = touching != 0;
    const float x = BananoClampSigned(handX);
    const float y = BananoClampSigned(handY);

    switch (control) {
    case BananoMarkerControl::LeftThumbstick:
        BananoVRRuntimeSetThumbstick(0, x, y);
        BananoPublishInputEvent(0, BananoInputKind::Thumbstick,
            static_cast<int>(control), x, y, 0.0f, pressed);
        break;
    case BananoMarkerControl::RightThumbstick:
        BananoVRRuntimeSetThumbstick(1, x, y);
        BananoPublishInputEvent(1, BananoInputKind::Thumbstick,
            static_cast<int>(control), x, y, 0.0f, pressed);
        break;
    case BananoMarkerControl::LeftTrigger:
        BananoVRRuntimeSetTrigger(0, BananoClampUnit(handY));
        BananoPublishInputEvent(0, BananoInputKind::Trigger,
            static_cast<int>(control), x, y, BananoClampUnit(handY), pressed);
        break;
    case BananoMarkerControl::RightTrigger:
        BananoVRRuntimeSetTrigger(1, BananoClampUnit(handY));
        BananoPublishInputEvent(1, BananoInputKind::Trigger,
            static_cast<int>(control), x, y, BananoClampUnit(handY), pressed);
        break;
    case BananoMarkerControl::LeftGrip:
        BananoVRRuntimeSetGrip(0, BananoClampUnit(handY));
        BananoPublishInputEvent(0, BananoInputKind::Grip,
            static_cast<int>(control), x, y, BananoClampUnit(handY), pressed);
        break;
    case BananoMarkerControl::RightGrip:
        BananoVRRuntimeSetGrip(1, BananoClampUnit(handY));
        BananoPublishInputEvent(1, BananoInputKind::Grip,
            static_cast<int>(control), x, y, BananoClampUnit(handY), pressed);
        break;
    case BananoMarkerControl::LeftThumbstickClick:
        BananoVRRuntimeSetThumbstickClick(0, pressed ? 1 : 0);
        BananoPublishInputEvent(0, BananoInputKind::ThumbstickClick,
            static_cast<int>(control), x, y, 0.0f, pressed);
        break;
    case BananoMarkerControl::RightThumbstickClick:
        BananoVRRuntimeSetThumbstickClick(1, pressed ? 1 : 0);
        BananoPublishInputEvent(1, BananoInputKind::ThumbstickClick,
            static_cast<int>(control), x, y, 0.0f, pressed);
        break;
    case BananoMarkerControl::LeftA:
    case BananoMarkerControl::LeftB:
    case BananoMarkerControl::LeftX:
    case BananoMarkerControl::LeftY:
    case BananoMarkerControl::RightA:
    case BananoMarkerControl::RightB:
    case BananoMarkerControl::RightX:
    case BananoMarkerControl::RightY: {
        const int controller = static_cast<int>(control) >=
            static_cast<int>(BananoMarkerControl::RightA) ? 1 : 0;
        BananoPublishInputEvent(controller, BananoInputKind::FaceButton,
            static_cast<int>(control), x, y, 0.0f, pressed);
        break;
    }
    case BananoMarkerControl::None:
        break;
    }
}

static void BananoPublishInputEvent(
    int controller,
    BananoInputKind kind,
    int control,
    float x,
    float y,
    float value,
    bool pressed) {
    g_lastInputEvent.controller = controller;
    g_lastInputEvent.kind = kind;
    g_lastInputEvent.control = control;
    g_lastInputEvent.valueX = x;
    g_lastInputEvent.valueY = y;
    g_lastInputEvent.value = value;
    g_lastInputEvent.pressed = pressed;
    ++g_inputSequence;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimePublishInput(
    int controller, int kind, int control,
    float x, float y, float value, int pressed) {
    if (controller < 0 || controller > 1)
        return;
    if (kind < 0 || kind > static_cast<int>(BananoInputKind::HmdPose))
        return;

    BananoPublishInputEvent(
        controller,
        static_cast<BananoInputKind>(kind),
        control,
        x,
        y,
        value,
        pressed != 0);
}

struct BananoThumbstick {
    float x;
    float y;
    bool click;
};

static BananoThumbstick g_leftThumbstick{ 0.0f, 0.0f, false };
static BananoThumbstick g_rightThumbstick{ 0.0f, 0.0f, false };

static BananoThumbstick& BananoGetThumbstick(int controller) {
    return controller == 0 ? g_leftThumbstick : g_rightThumbstick;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetThumbstick(
    int controller, float x, float y) {
    BananoThumbstick& stick = BananoGetThumbstick(controller);
    stick.x = x < -1.0f ? -1.0f : (x > 1.0f ? 1.0f : x);
    stick.y = y < -1.0f ? -1.0f : (y > 1.0f ? 1.0f : y);
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetThumbstickClick(
    int controller, int pressed) {
    BananoGetThumbstick(controller).click = pressed != 0;
}

struct BananoControllerAnalog {
    float trigger;
    float grip;
};

static BananoControllerAnalog g_leftAnalog{ 0.0f, 0.0f };
static BananoControllerAnalog g_rightAnalog{ 0.0f, 0.0f };

static BananoControllerAnalog& BananoGetAnalog(int controller) {
    return controller == 0 ? g_leftAnalog : g_rightAnalog;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetTrigger(
    int controller, float value) {
    BananoControllerAnalog& analog = BananoGetAnalog(controller);
    analog.trigger = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetGrip(
    int controller, float value) {
    BananoControllerAnalog& analog = BananoGetAnalog(controller);
    analog.grip = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

enum class BananoFaceButton { A, B, X, Y };

struct BananoControllerButtons {
    bool a;
    bool b;
    bool x;
    bool y;
};

static BananoControllerButtons g_leftButtons{ false, false, false, false };
static BananoControllerButtons g_rightButtons{ false, false, false, false };

static BananoControllerButtons& BananoGetButtons(int controller) {
    return controller == 0 ? g_leftButtons : g_rightButtons;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetFaceButton(
    int controller, int button, int pressed) {
    BananoControllerButtons& buttons = BananoGetButtons(controller);
    const bool value = pressed != 0;
    switch (static_cast<BananoFaceButton>(button)) {
    case BananoFaceButton::A: buttons.a = value; break;
    case BananoFaceButton::B: buttons.b = value; break;
    case BananoFaceButton::X: buttons.x = value; break;
    case BananoFaceButton::Y: buttons.y = value; break;
    }
}

static BananoControllerState g_rightController{
    false,
    { 0.20f, -0.10f, -0.45f, 0.0f, 0.0f, 0.0f, 1.0f }
};


static void BananoNormalizeQuaternion(BananoHmdPose& pose) {
    const float lengthSquared =
        pose.orientationX * pose.orientationX +
        pose.orientationY * pose.orientationY +
        pose.orientationZ * pose.orientationZ +
        pose.orientationW * pose.orientationW;

    if (lengthSquared <= 0.000001f) {
        pose.orientationX = 0.0f;
        pose.orientationY = 0.0f;
        pose.orientationZ = 0.0f;
        pose.orientationW = 1.0f;
        return;
    }

    const float inverseLength = 1.0f / std::sqrt(lengthSquared);
    pose.orientationX *= inverseLength;
    pose.orientationY *= inverseLength;
    pose.orientationZ *= inverseLength;
    pose.orientationW *= inverseLength;
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetLeftControllerOrientation(
    float qx, float qy, float qz, float qw) {
    g_leftController.pose.orientationX = qx;
    g_leftController.pose.orientationY = qy;
    g_leftController.pose.orientationZ = qz;
    g_leftController.pose.orientationW = qw;
    BananoNormalizeQuaternion(g_leftController.pose);
}

extern "C" BANANO_EXPORT void BANANO_XR_CALL BananoVRRuntimeSetRightControllerOrientation(
    float qx, float qy, float qz, float qw) {
    g_rightController.pose.orientationX = qx;
    g_rightController.pose.orientationY = qy;
    g_rightController.pose.orientationZ = qz;
    g_rightController.pose.orientationW = qw;
    BananoNormalizeQuaternion(g_rightController.pose);
}


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

static XrResult BANANO_XR_CALL BananoEnumerateViewConfigurations(
    XrInstance instance,
    XrSystemId systemId,
    uint32_t capacityInput,
    uint32_t* countOutput,
    XrViewConfigurationType* types) {

    if (!instance || !countOutput || systemId != BANANO_XR_SYSTEM_ID)
        return XR_ERROR_RUNTIME_FAILURE;

    *countOutput = 1;
    if (capacityInput == 0 || !types)
        return XR_SUCCESS;

    if (capacityInput < 1)
        return XR_ERROR_SIZE_INSUFFICIENT;

    types[0] = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoGetViewConfigurationProperties(
    XrInstance instance,
    XrSystemId systemId,
    XrViewConfigurationType type,
    XrViewConfigurationProperties* properties) {

    if (!instance || !properties || systemId != BANANO_XR_SYSTEM_ID)
        return XR_ERROR_RUNTIME_FAILURE;

    if (type != XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO)
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;

    if (properties->type != XR_TYPE_VIEW_CONFIGURATION_PROPERTIES)
        return XR_ERROR_RUNTIME_FAILURE;

    properties->viewConfigurationType = type;
    properties->fovMutable = 0;
    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoEnumerateViewConfigurationViews(
    XrInstance instance,
    XrSystemId systemId,
    XrViewConfigurationType type,
    uint32_t capacityInput,
    uint32_t* countOutput,
    XrViewConfigurationView* views) {

    if (!instance || !countOutput || systemId != BANANO_XR_SYSTEM_ID)
        return XR_ERROR_RUNTIME_FAILURE;

    if (type != XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO)
        return XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED;

    *countOutput = kBananoStereoViewCount;
    if (capacityInput == 0 || !views)
        return XR_SUCCESS;

    if (capacityInput < kBananoStereoViewCount)
        return XR_ERROR_SIZE_INSUFFICIENT;

    for (uint32_t i = 0; i < kBananoStereoViewCount; ++i) {
        if (views[i].type != XR_TYPE_VIEW_CONFIGURATION_VIEW)
            return XR_ERROR_RUNTIME_FAILURE;

        views[i].recommendedImageRectWidth = kBananoRecommendedEyeWidth;
        views[i].maxImageRectWidth = kBananoRecommendedEyeWidth;
        views[i].recommendedImageRectHeight = kBananoRecommendedEyeHeight;
        views[i].maxImageRectHeight = kBananoRecommendedEyeHeight;
        views[i].recommendedSwapchainSampleCount = 1;
        views[i].maxSwapchainSampleCount = 1;
    }

    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoGetSystemProperties(
    XrInstance instance,
    XrSystemId systemId,
    XrSystemProperties* properties) {

    if (!instance || !properties)
        return XR_ERROR_RUNTIME_FAILURE;

    if (systemId != BANANO_XR_SYSTEM_ID ||
        properties->type != XR_TYPE_SYSTEM_PROPERTIES)
        return XR_ERROR_RUNTIME_FAILURE;

    properties->systemId = BANANO_XR_SYSTEM_ID;
    properties->vendorId = 0xBABA;
    std::strncpy(
        properties->systemName,
        "Banano VR",
        XR_MAX_SYSTEM_NAME_SIZE - 1);
    properties->systemName[XR_MAX_SYSTEM_NAME_SIZE - 1] = '\0';

    properties->graphicsProperties.maxSwapchainImageWidth =
        kBananoRecommendedEyeWidth;
    properties->graphicsProperties.maxSwapchainImageHeight =
        kBananoRecommendedEyeHeight;
    properties->graphicsProperties.maxLayerCount =
        XR_MIN_COMPOSITION_LAYERS_SUPPORTED;

    properties->trackingProperties.orientationTracking = 1;
    properties->trackingProperties.positionTracking = 1;

    return XR_SUCCESS;
}

static XrResult BANANO_XR_CALL BananoGetSystem(
    XrInstance instance,
    const XrSystemGetInfo* getInfo,
    XrSystemId* systemId) {

    if (!instance || !getInfo || !systemId)
        return XR_ERROR_RUNTIME_FAILURE;

    if (getInfo->type != XR_TYPE_SYSTEM_GET_INFO ||
        getInfo->formFactor != XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY)
        return XR_ERROR_FORM_FACTOR_UNSUPPORTED;

    *systemId = BANANO_XR_SYSTEM_ID;
    return XR_SUCCESS;
}

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

    if (strcmp(name, "xrEnumerateViewConfigurations") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoEnumerateViewConfigurations);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrGetViewConfigurationProperties") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoGetViewConfigurationProperties);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrEnumerateViewConfigurationViews") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoEnumerateViewConfigurationViews);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrGetSystemProperties") == 0) {\n        *function = reinterpret_cast<PFN_xrVoidFunction>(\n            BananoGetSystemProperties);\n        return XR_SUCCESS;\n    }\n\n    if (strcmp(name, "xrGetSystem") == 0) {\n        *function = reinterpret_cast<PFN_xrVoidFunction>(\n            BananoGetSystem);\n        return XR_SUCCESS;\n    }\n\n    if (strcmp(name, "xrCreateSwapchain") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoCreateSwapchain);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrEnumerateSwapchainImages") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoEnumerateSwapchainImages);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrCreateSwapchain") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoCreateSwapchain);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrEnumerateSwapchainImages") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoEnumerateSwapchainImages);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrAcquireSwapchainImage") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoAcquireSwapchainImage);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrWaitSwapchainImage") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoWaitSwapchainImage);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrReleaseSwapchainImage") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoReleaseSwapchainImage);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrEndFrame") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoEndFrame);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrWaitFrame") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoWaitFrame);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrBeginFrame") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoBeginFrame);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrBeginSession") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoBeginSession);
        return XR_SUCCESS;
    }

    if (strcmp(name, "xrCreateSession") == 0) {
        *function = reinterpret_cast<PFN_xrVoidFunction>(
            BananoCreateSession);
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
