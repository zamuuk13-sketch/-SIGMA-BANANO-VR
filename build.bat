@echo off
setlocal

if not exist build mkdir build

where g++ >nul 2>nul
if errorlevel 1 (
    echo [ERRO] g++ nao encontrado no PATH.
    echo Instale o MinGW-w64 e adicione o binario ao PATH.
    pause
    exit /b 1
)

echo [BANANO VR] Compilando...

g++ -std=c++17 -O2 -mwindows src\main.cpp src\vr_runtime.cpp -lvfw32 -o build\BananoVR.exe

if errorlevel 1 (
    echo.
    echo [ERRO] Falha na compilacao.
    echo Verifique se o MinGW-w64 possui as bibliotecas do Windows/VFW.
    pause
    exit /b 1
)

echo.
echo [OK] build\BananoVR.exe criado com sucesso.
pause
