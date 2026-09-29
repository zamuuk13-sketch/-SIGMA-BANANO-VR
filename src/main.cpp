#include <windows.h>
#include <vfw.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#pragma comment(lib, "vfw32.lib")

#define IDC_IP 101
#define IDC_PORT 102
#define IDC_STATUS 103
#define IDC_SAVE 104
#define IDC_MARKER_ID 105
#define IDC_MARKER_COLOR 106
#define IDC_MARKER_X 107
#define IDC_MARKER_Y 108
#define IDC_ADD_MARKER 109
#define IDC_MARKER_LIST 110
#define IDC_MAPPING_MARKER 111
#define IDC_MAPPING_CONTROL 112
#define IDC_MAP 113
#define IDC_MAPPING_LIST 114
#define IDC_TOLERANCE 115
#define IDC_POSITION_TOLERANCE 116
#define IDC_APPLY_CALIBRATION 117
#define IDC_CAMERA 118
#define IDC_CAMERA_START 119
#define IDC_OVERLAY 120

struct Marker {
    int id;
    std::string color;
    int x;
    int y;
    std::string mapping;
};

struct HandTracking {
    bool detected;
    int minX;
    int minY;
    int maxX;
    int maxY;
    int palmX;
    int palmY;
    int fingertipX;
    int fingertipY;
};

struct Detection {
    std::string color;
    int x;
    int y;
    int size;
    int id;
};

static HWND g_ip = nullptr;
static HWND g_port = nullptr;
static HWND g_status = nullptr;
static HWND g_markerId = nullptr;
static HWND g_markerColor = nullptr;
static HWND g_markerX = nullptr;
static HWND g_markerY = nullptr;
static HWND g_markerList = nullptr;
static HWND g_mappingMarker = nullptr;
static HWND g_mappingControl = nullptr;
static HWND g_mappingList = nullptr;
static HWND g_tolerance = nullptr;
static HWND g_positionTolerance = nullptr;
static HWND g_camera = nullptr;
static HWND g_overlay = nullptr;
static HWND g_mainWindow = nullptr;

static std::vector<Marker> g_markers;
static std::vector<Detection> g_detections;
static int g_colorTolerance = 30;
static int g_positionToleranceValue = 20;
static bool g_cameraRunning = false;
static int g_cameraWidth = 640;
static int g_cameraHeight = 480;
static std::vector<Detection> g_trackedDetections;
static HandTracking g_handTracking{false, 0, 0, 0, 0, 0, 0, 0, 0};

static const char* kControls[] = {
    "Esquerdo: A", "Esquerdo: B", "Esquerdo: X", "Esquerdo: Y",
    "Esquerdo: Trigger", "Esquerdo: Grip", "Esquerdo: Analógico",
    "Esquerdo: Click Analógico",
    "Direito: A", "Direito: B", "Direito: X", "Direito: Y",
    "Direito: Trigger", "Direito: Grip", "Direito: Analógico",
    "Direito: Click Analógico"
};

static void SetStatus(const char* text) {
    SetWindowTextA(g_status, text);
}

static int FindMarkerIndex(int id) {
    for (int i = 0; i < (int)g_markers.size(); ++i)
        if (g_markers[i].id == id) return i;
    return -1;
}

static void RefreshMarkerList() {
    SendMessageA(g_markerList, LB_RESETCONTENT, 0, 0);
    for (const Marker& marker : g_markers) {
        char line[180]{};
        wsprintfA(line, "ID %d | %s | X:%d Y:%d | %s",
            marker.id, marker.color.c_str(), marker.x, marker.y,
            marker.mapping.empty() ? "Sem mapeamento" : marker.mapping.c_str());
        SendMessageA(g_markerList, LB_ADDSTRING, 0, (LPARAM)line);
    }
}

static void RefreshMappingList() {
    SendMessageA(g_mappingList, LB_RESETCONTENT, 0, 0);
    for (const Marker& marker : g_markers) {
        if (!marker.mapping.empty()) {
            char line[160]{};
            wsprintfA(line, "ID %d -> %s", marker.id, marker.mapping.c_str());
            SendMessageA(g_mappingList, LB_ADDSTRING, 0, (LPARAM)line);
        }
    }
}

static bool ReadInteger(HWND control, int& value) {
    char buffer[32]{};
    GetWindowTextA(control, buffer, sizeof(buffer));
    if (buffer[0] == '\0') return false;
    char* end = nullptr;
    long result = strtol(buffer, &end, 10);
    if (*end != '\0') return false;
    value = (int)result;
    return true;
}

static bool IsColor(BYTE r, BYTE g, BYTE b, const char* color) {
    int t = g_colorTolerance * 2 + 10;
    if (strcmp(color, "Azul") == 0)
        return b > r + t && b > g + t;
    if (strcmp(color, "Vermelho") == 0)
        return r > g + t && r > b + t;
    if (strcmp(color, "Amarelo") == 0)
        return r > 120 && g > 120 && b + t < (r + g) / 2;
    if (strcmp(color, "Verde") == 0)
        return g > r + t && g > b + t;
    if (strcmp(color, "Roxo") == 0)
        return r > g + t / 2 && b > g + t / 2;
    if (strcmp(color, "Laranja") == 0)
        return r > 150 && g > 60 && g < r && b + t < g;
    if (strcmp(color, "Branco") == 0)
        return r > 180 - t && g > 180 - t && b > 180 - t;
    return false;
}

static bool IsSkin(BYTE r, BYTE g, BYTE b) {
    // Faixa simples de pele, usada somente como primeira camada de hand tracking.
    return r > 80 && g > 35 && b > 20 &&
           r > g + 15 && r > b + 25 &&
           (r - g) < 140;
}

static void DetectHand(LPVIDEOHDR frame) {
    if (!frame || !frame->lpData || !g_cameraRunning) return;

    BYTE* pixels = frame->lpData;
    const int width = g_cameraWidth;
    const int height = g_cameraHeight;
    const int stride = width * 3;

    long sumX = 0, sumY = 0, count = 0;
    int minX = width, minY = height, maxX = 0, maxY = 0;

    for (int y = 0; y < height; y += 4) {
        const int sourceY = height - 1 - y;
        BYTE* row = pixels + sourceY * stride;

        for (int x = 0; x < width; x += 4) {
            BYTE b = row[x * 3 + 0];
            BYTE g = row[x * 3 + 1];
            BYTE r = row[x * 3 + 2];

            if (!IsSkin(r, g, b)) continue;

            sumX += x;
            sumY += y;
            ++count;
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }

    if (count < 120) {
        g_handTracking.detected = false;
        return;
    }

    int centerX = (int)(sumX / count);
    int centerY = (int)(sumY / count);

    long bestDistance = -1;
    int tipX = centerX;
    int tipY = centerY;

    for (int y = minY; y <= maxY; y += 6) {
        const int sourceY = height - 1 - y;
        BYTE* row = pixels + sourceY * stride;

        for (int x = minX; x <= maxX; x += 6) {
            BYTE b = row[x * 3 + 0];
            BYTE g = row[x * 3 + 1];
            BYTE r = row[x * 3 + 2];

            if (!IsSkin(r, g, b)) continue;

            long dx = (long)x - centerX;
            long dy = (long)y - centerY;
            long distance = dx * dx + dy * dy;

            if (distance > bestDistance) {
                bestDistance = distance;
                tipX = x;
                tipY = y;
            }
        }
    }

    g_handTracking.detected = true;
    g_handTracking.minX = minX;
    g_handTracking.minY = minY;
    g_handTracking.maxX = maxX;
    g_handTracking.maxY = maxY;
    g_handTracking.palmX = centerX;
    g_handTracking.palmY = centerY;
    g_handTracking.fingertipX = tipX;
    g_handTracking.fingertipY = tipY;
}

static void DetectFrame(LPVIDEOHDR frame) {
    if (!frame || !frame->lpData || !g_cameraRunning) return;

    int width = g_cameraWidth;
    int height = g_cameraHeight;
    if (width <= 0 || height <= 0) return;

    DetectHand(frame);\n\n    // VFW callback data is the raw 24-bit frame buffer.
    BYTE* pixels = frame->lpData;
    const int bytesPerPixel = 3;
    const int stride = width * bytesPerPixel;

    const char* colors[] = {"Azul", "Vermelho", "Amarelo", "Verde", "Roxo", "Laranja", "Branco"};
    std::vector<Detection> next;

    // Lightweight sampling: enough for a first visual tracker without extra libraries.
    for (const char* color : colors) {
        long sx = 0, sy = 0, count = 0;
        int minX = width, minY = height, maxX = 0, maxY = 0;

        for (int y = 0; y < height; y += 6) {
            const int sourceY = height - 1 - y;
            BYTE* row = pixels + sourceY * stride;

            for (int x = 0; x < width; x += 6) {
                BYTE b = row[x * 3 + 0];
                BYTE g = row[x * 3 + 1];
                BYTE r = row[x * 3 + 2];

                if (!IsColor(r, g, b, color)) continue;

                sx += x; sy += y; ++count;
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }

        if (count >= 8) {
            Detection d;
            d.color = color;
            d.x = (int)(sx / count);
            d.y = (int)(sy / count);
            d.size = ((maxX - minX) + (maxY - minY)) / 4;
            if (d.size < 8) d.size = 8;
            next.push_back(d);
        }
    }

    for (Detection& d : next) {
        d.id = 0;
        int bestIndex = -1;
        long bestDistance = 0x7fffffff;
        for (int i = 0; i < (int)g_markers.size(); ++i) {
            const Marker& marker = g_markers[i];
            if (marker.color != d.color) continue;
            long dx = (long)d.x - marker.x;
            long dy = (long)d.y - marker.y;
            long distance = dx * dx + dy * dy;
            long tolerance = 30L + (long)g_positionToleranceValue * 6L;
            if (distance <= tolerance * tolerance && distance < bestDistance) {
                bestDistance = distance;
                bestIndex = i;
            }
        }
        if (bestIndex >= 0) d.id = g_markers[bestIndex].id;
    }

    for (Detection& current : next) {
        for (const Detection& previous : g_trackedDetections) {
            if (current.id > 0 && current.id == previous.id) {
                current.x = (current.x * 3 + previous.x) / 4;
                current.y = (current.y * 3 + previous.y) / 4;
                break;
            }
        }
    }

    g_detections = next;
    g_trackedDetections = next;
    if (g_mainWindow) InvalidateRect(g_mainWindow, nullptr, FALSE);
    if (g_overlay) InvalidateRect(g_overlay, nullptr, FALSE);
}

static void DrawTrackingOverlay(HDC dc) {
    if (g_handTracking.detected) {
        HPEN handPen = CreatePen(PS_SOLID, 2, RGB(80, 150, 255));
        HGDIOBJ oldPen = SelectObject(dc, handPen);
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));

        Rectangle(dc,
            g_handTracking.minX,
            g_handTracking.minY,
            g_handTracking.maxX,
            g_handTracking.maxY);

        SelectObject(dc, oldPen);
        DeleteObject(handPen);

        const int radius = 8;
        HPEN tipPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        HBRUSH tipBrush = CreateSolidBrush(RGB(255, 255, 255));
        oldPen = SelectObject(dc, tipPen);
        HGDIOBJ oldBrush2 = SelectObject(dc, tipBrush);
        Ellipse(dc,
            g_handTracking.fingertipX - radius,
            g_handTracking.fingertipY - radius,
            g_handTracking.fingertipX + radius,
            g_handTracking.fingertipY + radius);
        SelectObject(dc, oldBrush2);
        SelectObject(dc, oldPen);
        DeleteObject(tipPen);
        DeleteObject(tipBrush);
    }


    SetBkMode(dc, TRANSPARENT);

    for (const Detection& d : g_trackedDetections) {
        const int radius = 11;
        const int left = d.x - radius;
        const int top = d.y - radius;
        const int right = d.x + radius;
        const int bottom = d.y + radius;

        HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
        HGDIOBJ oldPen = SelectObject(dc, pen);
        HGDIOBJ oldBrush = SelectObject(dc, brush);

        Ellipse(dc, left, top, right, bottom);

        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        DeleteObject(pen);

        char label[32]{};
        if (d.id > 0)
            wsprintfA(label, "%d", d.id);
        else
            wsprintfA(label, "?");

        RECT textRect{left, top, right, bottom};
        DrawTextA(dc, label, -1, &textRect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);
        DrawTrackingOverlay(dc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK FrameCallback(HWND, LPVIDEOHDR frame) {
    DetectFrame(frame);
    return 0;
}

static void StartCamera() {
    if (g_cameraRunning) return;

    if (!capGetDriverDescriptionA(0, nullptr, 0, nullptr, 0)) {
        // Continue anyway; capCreateCaptureWindow will report if no camera exists.
    }

    g_camera = capCreateCaptureWindowA(
        "Banano VR Camera",
        WS_CHILD | WS_VISIBLE,
        430, 70, 640, 480,
        g_mainWindow, 500, GetModuleHandleA(nullptr)
    );

    if (!g_camera) {
        SetStatus("Status: nao foi possivel abrir a camera.");
        return;
    }

    capSetCallbackOnFrame(g_camera, FrameCallback);
    capPreviewRate(g_camera, 33);
    capPreview(g_camera, TRUE);

    g_overlay = CreateWindowExA(
        WS_EX_TRANSPARENT,
        "BananoVROverlay",
        "",
        WS_CHILD | WS_VISIBLE,
        0, 0, g_cameraWidth, g_cameraHeight,
        g_camera, (HMENU)IDC_OVERLAY, GetModuleHandleA(nullptr), nullptr
    );

    if (!g_overlay) {
        capPreview(g_camera, FALSE);
        capSetCallbackOnFrame(g_camera, nullptr);
        capDriverDisconnect(g_camera);
        DestroyWindow(g_camera);
        g_camera = nullptr;
        SetStatus("Status: nao foi possivel criar o overlay de tracking.");
        return;
    }

    SetWindowPos(g_overlay, HWND_TOP, 0, 0, g_cameraWidth, g_cameraHeight,
        SWP_SHOWWINDOW);
    g_cameraRunning = true;
    SetStatus("Status: camera ativa e procurando marcadores.");
}

static void StopCamera() {
    if (!g_camera) return;

    if (g_overlay) {
        DestroyWindow(g_overlay);
        g_overlay = nullptr;
    }

    capPreview(g_camera, FALSE);
    capSetCallbackOnFrame(g_camera, nullptr);
    capDriverDisconnect(g_camera);
    DestroyWindow(g_camera);
    g_camera = nullptr;
    g_cameraRunning = false;
    g_detections.clear();
    g_trackedDetections.clear();
    g_handTracking.detected = false;
    InvalidateRect(g_mainWindow, nullptr, FALSE);
    SetStatus("Status: camera parada.");
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_mainWindow = hwnd;
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        CreateWindowA("STATIC", "Banano VR PC - Etapa 9",
            WS_CHILD | WS_VISIBLE, 20, 15, 380, 25, hwnd, nullptr, nullptr, nullptr);

        CreateWindowA("STATIC", "IP do celular:",
            WS_CHILD | WS_VISIBLE, 20, 50, 100, 20, hwnd, nullptr, nullptr, nullptr);
        g_ip = CreateWindowA("EDIT", "127.0.0.1",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 72, 170, 24, hwnd, (HMENU)IDC_IP, nullptr, nullptr);

        CreateWindowA("STATIC", "Porta:",
            WS_CHILD | WS_VISIBLE, 205, 50, 60, 20, hwnd, nullptr, nullptr, nullptr);
        g_port = CreateWindowA("EDIT", "8765",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            205, 72, 90, 24, hwnd, (HMENU)IDC_PORT, nullptr, nullptr);

        CreateWindowA("BUTTON", "Salvar",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            305, 72, 65, 24, hwnd, (HMENU)IDC_SAVE, nullptr, nullptr);

        CreateWindowA("STATIC", "Camera / hand tracking",
            WS_CHILD | WS_VISIBLE, 430, 15, 300, 25, hwnd, nullptr, nullptr, nullptr);
        CreateWindowA("BUTTON", "Iniciar camera",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            430, 555, 130, 30, hwnd, (HMENU)IDC_CAMERA_START, nullptr, nullptr);

        CreateWindowA("STATIC", "Novo marcador",
            WS_CHILD | WS_VISIBLE, 20, 112, 150, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindowA("STATIC", "ID:",
            WS_CHILD | WS_VISIBLE, 20, 138, 30, 20, hwnd, nullptr, nullptr, nullptr);
        g_markerId = CreateWindowA("EDIT", "1",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            48, 136, 55, 24, hwnd, (HMENU)IDC_MARKER_ID, nullptr, nullptr);

        CreateWindowA("STATIC", "Cor:",
            WS_CHILD | WS_VISIBLE, 115, 138, 35, 20, hwnd, nullptr, nullptr, nullptr);
        g_markerColor = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
            150, 136, 100, 120, hwnd, (HMENU)IDC_MARKER_COLOR, nullptr, nullptr);

        const char* colors[] = {"Azul", "Vermelho", "Amarelo", "Verde", "Roxo", "Laranja", "Branco"};
        for (const char* color : colors)
            SendMessageA(g_markerColor, CB_ADDSTRING, 0, (LPARAM)color);
        SendMessageA(g_markerColor, CB_SETCURSEL, 0, 0);

        CreateWindowA("STATIC", "X:",
            WS_CHILD | WS_VISIBLE, 265, 138, 20, 20, hwnd, nullptr, nullptr, nullptr);
        g_markerX = CreateWindowA("EDIT", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            285, 136, 45, 24, hwnd, (HMENU)IDC_MARKER_X, nullptr, nullptr);

        CreateWindowA("STATIC", "Y:",
            WS_CHILD | WS_VISIBLE, 335, 138, 20, 20, hwnd, nullptr, nullptr, nullptr);
        g_markerY = CreateWindowA("EDIT", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            350, 136, 45, 24, hwnd, (HMENU)IDC_MARKER_Y, nullptr, nullptr);

        CreateWindowA("BUTTON", "Adicionar marcador",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 172, 160, 28, hwnd, (HMENU)IDC_ADD_MARKER, nullptr, nullptr);

        CreateWindowA("STATIC", "Marcadores:",
            WS_CHILD | WS_VISIBLE, 20, 210, 130, 20, hwnd, nullptr, nullptr, nullptr);
        g_markerList = CreateWindowA("LISTBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            20, 232, 375, 90, hwnd, (HMENU)IDC_MARKER_LIST, nullptr, nullptr);

        CreateWindowA("STATIC", "Calibracao",
            WS_CHILD | WS_VISIBLE, 20, 330, 150, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindowA("STATIC", "Tolerancia de cor (0-100):",
            WS_CHILD | WS_VISIBLE, 20, 354, 155, 20, hwnd, nullptr, nullptr, nullptr);
        g_tolerance = CreateWindowA("EDIT", "30",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            180, 352, 55, 24, hwnd, (HMENU)IDC_TOLERANCE, nullptr, nullptr);

        CreateWindowA("STATIC", "Tolerancia de posicao (0-100):",
            WS_CHILD | WS_VISIBLE, 20, 384, 170, 20, hwnd, nullptr, nullptr, nullptr);
        g_positionTolerance = CreateWindowA("EDIT", "20",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            195, 382, 55, 24, hwnd, (HMENU)IDC_POSITION_TOLERANCE, nullptr, nullptr);
        CreateWindowA("BUTTON", "Aplicar calibracao",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            270, 352, 125, 28, hwnd, (HMENU)IDC_APPLY_CALIBRATION, nullptr, nullptr);

        CreateWindowA("STATIC", "Mapeamento de controle",
            WS_CHILD | WS_VISIBLE, 20, 420, 220, 20, hwnd, nullptr, nullptr, nullptr);
        CreateWindowA("STATIC", "ID:",
            WS_CHILD | WS_VISIBLE, 20, 446, 30, 20, hwnd, nullptr, nullptr, nullptr);
        g_mappingMarker = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
            48, 444, 70, 120, hwnd, (HMENU)IDC_MAPPING_MARKER, nullptr, nullptr);

        CreateWindowA("STATIC", "Controle:",
            WS_CHILD | WS_VISIBLE, 128, 446, 65, 20, hwnd, nullptr, nullptr, nullptr);
        g_mappingControl = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
            195, 444, 180, 180, hwnd, (HMENU)IDC_MAPPING_CONTROL, nullptr, nullptr);
        for (const char* control : kControls)
            SendMessageA(g_mappingControl, CB_ADDSTRING, 0, (LPARAM)control);
        SendMessageA(g_mappingControl, CB_SETCURSEL, 0, 0);

        CreateWindowA("BUTTON", "Mapear",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 480, 100, 28, hwnd, (HMENU)IDC_MAP, nullptr, nullptr);
        g_mappingList = CreateWindowA("LISTBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
            20, 515, 355, 55, hwnd, (HMENU)IDC_MAPPING_LIST, nullptr, nullptr);

        g_status = CreateWindowA("STATIC", "Status: pronto.",
            WS_CHILD | WS_VISIBLE, 20, 575, 390, 22,
            hwnd, (HMENU)IDC_STATUS, nullptr, nullptr);

        const HWND controls[] = {
            g_ip, g_port, g_markerId, g_markerColor, g_markerX, g_markerY,
            g_markerList, g_tolerance, g_positionTolerance, g_mappingMarker,
            g_mappingControl, g_mappingList, g_status
        };
        for (HWND control : controls)
            SendMessageA(control, WM_SETFONT, (WPARAM)font, TRUE);

        break;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_SAVE) {
            char ip[64]{}, port[16]{};
            GetWindowTextA(g_ip, ip, sizeof(ip));
            GetWindowTextA(g_port, port, sizeof(port));
            SetStatus((ip[0] == '\0' || port[0] == '\0')
                ? "Status: preencha IP e porta."
                : "Status: configuracao salva.");
        }

        if (LOWORD(wParam) == IDC_CAMERA_START) {
            if (g_cameraRunning) StopCamera();
            else StartCamera();
        }

        if (LOWORD(wParam) == IDC_ADD_MARKER) {
            int id = 0, x = 0, y = 0;
            if (!ReadInteger(g_markerId, id) || !ReadInteger(g_markerX, x) ||
                !ReadInteger(g_markerY, y) || id <= 0) {
                SetStatus("Status: ID, X e Y precisam ser validos.");
                break;
            }

            int colorIndex = (int)SendMessageA(g_markerColor, CB_GETCURSEL, 0, 0);
            if (colorIndex == CB_ERR) {
                SetStatus("Status: selecione uma cor.");
                break;
            }

            char color[32]{};
            SendMessageA(g_markerColor, CB_GETLBTEXT, colorIndex, (LPARAM)color);
            if (FindMarkerIndex(id) >= 0) {
                SetStatus("Status: esse ID ja esta cadastrado.");
                break;
            }

            g_markers.push_back({id, color, x, y, ""});
            RefreshMarkerList();

            char idText[16]{};
            wsprintfA(idText, "%d", id);
            SendMessageA(g_mappingMarker, CB_ADDSTRING, 0, (LPARAM)idText);
            SetStatus("Status: marcador adicionado.");
        }

        if (LOWORD(wParam) == IDC_APPLY_CALIBRATION) {
            int colorTolerance = 0, positionTolerance = 0;
            if (!ReadInteger(g_tolerance, colorTolerance) ||
                !ReadInteger(g_positionTolerance, positionTolerance) ||
                colorTolerance < 0 || colorTolerance > 100 ||
                positionTolerance < 0 || positionTolerance > 100) {
                SetStatus("Status: use valores de 0 a 100.");
                break;
            }
            g_colorTolerance = colorTolerance;
            g_positionToleranceValue = positionTolerance;
            SetStatus("Status: calibracao aplicada.");
        }

        if (LOWORD(wParam) == IDC_MAP) {
            int markerSelection = (int)SendMessageA(g_mappingMarker, CB_GETCURSEL, 0, 0);
            int controlSelection = (int)SendMessageA(g_mappingControl, CB_GETCURSEL, 0, 0);
            if (markerSelection == CB_ERR || controlSelection == CB_ERR) {
                SetStatus("Status: selecione marcador e controle.");
                break;
            }

            char idText[16]{};
            SendMessageA(g_mappingMarker, CB_GETLBTEXT, markerSelection, (LPARAM)idText);
            int index = FindMarkerIndex(atoi(idText));
            if (index < 0) {
                SetStatus("Status: marcador nao encontrado.");
                break;
            }

            char control[80]{};
            SendMessageA(g_mappingControl, CB_GETLBTEXT, controlSelection, (LPARAM)control);

            for (int i = 0; i < (int)g_markers.size(); ++i) {
                if (i != index && g_markers[i].mapping == control) {
                    SetStatus("Status: esse controle ja esta mapeado.");
                    return 0;
                }
            }

            g_markers[index].mapping = control;
            RefreshMarkerList();
            RefreshMappingList();
            SetStatus("Status: controle mapeado.");
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);

        if (g_cameraRunning) {
            int recognized = 0;
            for (const Detection& d : g_detections) if (d.id > 0) ++recognized;
            char info[128]{};
            wsprintfA(info, "Detectados: %d | IDs reconhecidos: %d", (int)g_detections.size(), recognized);
            SetBkMode(dc, TRANSPARENT);
            TextOutA(dc, 430, 535, info, (int)strlen(info));

            int lineY = 555;
            for (const Detection& d : g_detections) {
                char line[128]{};
                if (d.id > 0)
                    wsprintfA(line, "ID %d | %s | X:%d Y:%d", d.id, d.color.c_str(), d.x, d.y);
                else
                    wsprintfA(line, "Sem ID | %s | X:%d Y:%d", d.color.c_str(), d.x, d.y);
                TextOutA(dc, 430, lineY, line, (int)strlen(line));
                lineY += 18;
            }
        }

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        StopCamera();
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const char* className = "BananoVRWindow";
    WNDCLASSA wc{};
    WNDCLASSA overlayClass{};
    overlayClass.lpfnWndProc = OverlayProc;
    overlayClass.hInstance = hInstance;
    overlayClass.lpszClassName = "BananoVROverlay";
    overlayClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    overlayClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);

    if (!RegisterClassA(&overlayClass)) {
        MessageBoxA(nullptr, "Nao foi possivel iniciar o overlay de tracking.", "Banano VR", MB_OK | MB_ICONERROR);
        return 1;
    }

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassA(&wc)) {
        MessageBoxA(nullptr, "Nao foi possivel iniciar a janela.", "Banano VR", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowA(
        className,
        "Banano VR PC",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 1100, 640,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) {
        MessageBoxA(nullptr, "Nao foi possivel criar a janela.", "Banano VR", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}
