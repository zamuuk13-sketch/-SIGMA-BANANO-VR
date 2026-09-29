#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    MessageBoxA(
        nullptr,
        "Banano VR PC - Etapa 1\n\nAplicacao iniciada com sucesso.",
        "Banano VR",
        MB_OK | MB_ICONINFORMATION
    );
    return 0;
}
