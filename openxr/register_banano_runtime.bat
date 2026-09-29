@echo off
setlocal

set "MANIFEST=%~dp0banano_vr_runtime.json"

if not exist "%MANIFEST%" (
    echo [ERRO] Manifesto OpenXR nao encontrado.
    pause
    exit /b 1
)

echo [BANANO VR] Registrando runtime como disponivel no Windows...
reg add "HKCU\Software\Khronos\OpenXR\1\AvailableRuntimes" /v "%MANIFEST%" /t REG_DWORD /d 0 /f >nul

if errorlevel 1 (
    echo [ERRO] Nao foi possivel registrar o runtime.
    pause
    exit /b 1
)

echo [OK] Banano VR foi registrado como runtime OpenXR disponivel.
echo [INFO] O runtime ativo do Windows NAO foi alterado nesta etapa.
pause
