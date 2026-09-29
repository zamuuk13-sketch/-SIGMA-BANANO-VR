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
#define IDC_MAPPING_MARKER 111
#define IDC_MAPPING_CONTROL 112
#define IDC_MAP 113
#define IDC_MAPPING_LIST 114

struct Marker {
    int id;
    std::string color;
    int x;
    int y;
    std::string mapping;
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

static std::vector<Marker> g_markers;

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
    for (int i = 0; i < (int)g_markers.size(); ++i) {
        if (g_markers[i].id == id) {
            return i;
        }
    }
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

        CreateWindowA("STATIC", "Banano VR PC - Etapa 4",
            WS_CHILD | WS_VISIBLE, 20, 15, 380, 25,
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

        CreateWindowA("STATIC", "Marcadores:",
            WS_CHILD | WS_VISIBLE, 20, 210, 130, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_markerList = CreateWindowA("LISTBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            20, 232, 375, 95, hwnd, (HMENU)IDC_MARKER_LIST, nullptr, nullptr);

        CreateWindowA("STATIC", "Mapeamento de controle",
            WS_CHILD | WS_VISIBLE, 20, 340, 220, 20,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowA("STATIC", "ID:",
            WS_CHILD | WS_VISIBLE, 20, 366, 30, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_mappingMarker = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
            48, 364, 70, 120, hwnd, (HMENU)IDC_MAPPING_MARKER, nullptr, nullptr);

        CreateWindowA("STATIC", "Controle:",
            WS_CHILD | WS_VISIBLE, 128, 366, 65, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_mappingControl = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST,
            195, 364, 180, 180, hwnd, (HMENU)IDC_MAPPING_CONTROL, nullptr, nullptr);

        for (const char* control : kControls) {
            SendMessageA(g_mappingControl, CB_ADDSTRING, 0, (LPARAM)control);
        }
        SendMessageA(g_mappingControl, CB_SETCURSEL, 0, 0);

        CreateWindowA("BUTTON", "Mapear",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 400, 100, 28, hwnd, (HMENU)IDC_MAP, nullptr, nullptr);

        g_mappingList = CreateWindowA("LISTBOX", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
            20, 438, 355, 80, hwnd, (HMENU)IDC_MAPPING_LIST, nullptr, nullptr);

        g_status = CreateWindowA("STATIC", "Status: pronto.",
            WS_CHILD | WS_VISIBLE, 20, 528, 390, 22,
            hwnd, (HMENU)IDC_STATUS, nullptr, nullptr);

        SendMessageA(g_ip, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_port, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerId, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerColor, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerX, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerY, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_markerList, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_mappingMarker, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_mappingControl, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_mappingList, WM_SETFONT, (WPARAM)font, TRUE);
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

            g_markers.push_back({ id, color, x, y, "" });
            RefreshMarkerList();

            char idText[16]{};
            wsprintfA(idText, "%d", id);
            SendMessageA(g_mappingMarker, CB_ADDSTRING, 0, (LPARAM)idText);

            SetStatus("Status: marcador adicionado.");
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

            int id = atoi(idText);
            int index = FindMarkerIndex(id);

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
        CW_USEDEFAULT, CW_USEDEFAULT, 440, 580,
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
