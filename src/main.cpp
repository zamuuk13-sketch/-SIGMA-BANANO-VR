#include <windows.h>
#include <vector>
#include <string>

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

struct Marker {
    int id;
    std::string color;
    int x;
    int y;
};

static HWND g_ip = nullptr;
static HWND g_port = nullptr;
static HWND g_status = nullptr;
static HWND g_markerId = nullptr;
static HWND g_markerColor = nullptr;
static HWND g_markerX = nullptr;
static HWND g_markerY = nullptr;
static HWND g_markerList = nullptr;

static std::vector<Marker> g_markers;

static void SetStatus(const char* text) {
    SetWindowTextA(g_status, text);
}

static void RefreshMarkerList() {
    SendMessageA(g_markerList, LB_RESETCONTENT, 0, 0);

    for (const Marker& marker : g_markers) {
        char line[160]{};
        wsprintfA(line, "ID %d | %s | X:%d Y:%d",
            marker.id, marker.color.c_str(), marker.x, marker.y);
        SendMessageA(g_markerList, LB_ADDSTRING, 0, (LPARAM)line);
    }
}

static bool ReadInteger(HWND control, int& value) {
    char buffer[32]{};
    GetWindowTextA(control, buffer, sizeof(buffer));

    if (buffer[0] == '\0') {
        return false;
    }

    char* end = nullptr;
    long result = strtol(buffer, &end, 10);

    if (*end != '\0') {
        return false;
    }

    value = (int)result;
    return true;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        CreateWindowA("STATIC", "Banano VR PC - Etapa 3",
            WS_CHILD | WS_VISIBLE, 20, 15, 330, 25,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowA("STATIC", "IP do celular:",
            WS_CHILD | WS_VISIBLE, 20, 50, 100, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_ip = CreateWindowA("EDIT", "127.0.0.1",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 72, 170, 24, hwnd, (HMENU)IDC_IP, nullptr, nullptr);

        CreateWindowA("STATIC", "Porta:",
            WS_CHILD | WS_VISIBLE, 205, 50, 60, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_port = CreateWindowA("EDIT", "8765",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            205, 72, 90, 24, hwnd, (HMENU)IDC_PORT, nullptr, nullptr);

        CreateWindowA("BUTTON", "Salvar",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            305, 72, 65, 24, hwnd, (HMENU)IDC_SAVE, nullptr, nullptr);

        CreateWindowA("STATIC", "Novo marcador",
            WS_CHILD | WS_VISIBLE, 20, 112, 150, 20,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowA("STATIC", "ID:",
            WS_CHILD | WS_VISIBLE, 20, 138, 30, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_markerId = CreateWindowA("EDIT", "1",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            48, 136, 55, 24, hwnd, (HMENU)IDC_MARKER_ID, nullptr, nullptr);

        CreateWindowA("STATIC", "Cor:",
            WS_CHILD | WS_VISIBLE, 115, 138, 35, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_markerColor = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
            150, 136, 100, 120, hwnd, (HMENU)IDC_MARKER_COLOR, nullptr, nullptr);

        const char* colors[] = {
            "Azul", "Vermelho", "Amarelo", "Verde", "Roxo", "Laranja", "Branco"
        };

        for (const char* color : colors) {
            SendMessageA(g_markerColor, CB_ADDSTRING, 0, (LPARAM)color);
        }

        SendMessageA(g_markerColor, CB_SETCURSEL, 0, 0);

        CreateWindowA("STATIC", "X:",
            WS_CHILD | WS_VISIBLE, 265, 138, 20, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_markerX = CreateWindowA("EDIT", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            285, 136, 45, 24, hwnd, (HMENU)IDC_MARKER_X, nullptr, nullptr);

        CreateWindowA("STATIC", "Y:",
            WS_CHILD | WS_VISIBLE, 335, 138, 20, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_markerY = CreateWindowA("EDIT", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            350, 136, 45, 24, hwnd, (HMENU)IDC_MARKER_Y, nullptr, nullptr);

        CreateWindowA("BUTTON", "Adicionar marcador",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 172, 160, 28, hwnd, (HMENU)IDC_ADD_MARKER, nullptr, nullptr);

        CreateWindowA("STATIC", "Marcadores cadastrados:",
            WS_CHILD | WS_VISIBLE, 20, 210, 180, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_markerList = CreateWindowA("LISTBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            20, 232, 375, 105, hwnd, (HMENU)IDC_MARKER_LIST, nullptr, nullptr);

        g_status = CreateWindowA("STATIC", "Status: pronto.",
            WS_CHILD | WS_VISIBLE, 20, 345, 375, 22,
            hwnd, (HMENU)IDC_STATUS, nullptr, nullptr);

        SendMessageA(g_ip, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_port, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerId, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerColor, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerX, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerY, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerList, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_status, WM_SETFONT, (WPARAM)font, TRUE);
        break;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_SAVE) {
            char ip[64]{};
            char port[16]{};

            GetWindowTextA(g_ip, ip, sizeof(ip));
            GetWindowTextA(g_port, port, sizeof(port));

            if (ip[0] == '\0' || port[0] == '\0') {
                SetStatus("Status: preencha IP e porta.");
            } else {
                SetStatus("Status: configuracao salva.");
            }
        }

        if (LOWORD(wParam) == IDC_ADD_MARKER) {
            int id = 0;
            int x = 0;
            int y = 0;

            if (!ReadInteger(g_markerId, id) ||
                !ReadInteger(g_markerX, x) ||
                !ReadInteger(g_markerY, y) ||
                id <= 0) {
                SetStatus("Status: ID, X e Y precisam ser validos.");
                break;
            }

            if (SendMessageA(g_markerColor, CB_GETCURSEL, 0, 0) == CB_ERR) {
                SetStatus("Status: selecione uma cor.");
                break;
            }

            char color[32]{};
            SendMessageA(
                g_markerColor,
                CB_GETLBTEXT,
                SendMessageA(g_markerColor, CB_GETCURSEL, 0, 0),
                (LPARAM)color
            );

            for (const Marker& marker : g_markers) {
                if (marker.id == id) {
                    SetStatus("Status: esse ID ja esta cadastrado.");
                    return 0;
                }
            }

            g_markers.push_back({ id, color, x, y });
            RefreshMarkerList();
            SetStatus("Status: marcador adicionado.");
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const char* className = "BananoVRWindow";

    WNDCLASSA wc{};
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
        CW_USEDEFAULT, CW_USEDEFAULT, 430, 420,
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
