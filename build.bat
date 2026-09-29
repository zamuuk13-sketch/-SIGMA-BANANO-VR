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

g++ -std=c++17 -O2 -mwindows src\main.cpp -o build\BananoVR.exe

if errorlevel 1 (
    echo.
    echo [ERRO] Falha na compilacao.
    pause
    exit /b 1
)

echo.
echo [OK] build\BananoVR.exe criado com sucesso.
pause
