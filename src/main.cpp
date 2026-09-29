#include <windows.h>

#define IDC_IP 101
#define IDC_PORT 102
#define IDC_STATUS 103
#define IDC_SAVE 104

static HWND g_ip = nullptr;
static HWND g_port = nullptr;
static HWND g_status = nullptr;

static void SetStatus(const char* text) {
    SetWindowTextA(g_status, text);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        CreateWindowA("STATIC", "Banano VR PC - Etapa 2",
            WS_CHILD | WS_VISIBLE, 20, 15, 330, 25,
            hwnd, nullptr, nullptr, nullptr);

        CreateWindowA("STATIC", "Endereco IP do celular:",
            WS_CHILD | WS_VISIBLE, 20, 55, 170, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_ip = CreateWindowA("EDIT", "127.0.0.1",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 78, 210, 25, hwnd, (HMENU)IDC_IP, nullptr, nullptr);

        CreateWindowA("STATIC", "Porta:",
            WS_CHILD | WS_VISIBLE, 20, 115, 80, 20,
            hwnd, nullptr, nullptr, nullptr);

        g_port = CreateWindowA("EDIT", "8765",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
            20, 138, 120, 25, hwnd, (HMENU)IDC_PORT, nullptr, nullptr);

        CreateWindowA("BUTTON", "Salvar configuracao",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 180, 210, 30, hwnd, (HMENU)IDC_SAVE, nullptr, nullptr);

        g_status = CreateWindowA("STATIC", "Status: configuracao inicial",
            WS_CHILD | WS_VISIBLE, 20, 225, 330, 25,
            hwnd, (HMENU)IDC_STATUS, nullptr, nullptr);

        SendMessageA(g_ip, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageA(g_port, WM_SETFONT, (WPARAM)font, TRUE);
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
        CW_USEDEFAULT, CW_USEDEFAULT, 390, 310,
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
