@echo off
REM ESSENCE build script - run from a normal Command Prompt (needs Visual Studio 2022 + CMake + Git)
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 || goto :err
cmake --build build --config Release --parallel || goto :err

set ISCC="%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if exist %ISCC% (
    %ISCC% installer\Essence.iss || goto :err
    echo.
    echo Done! Installer: installer\Output\Essence_Setup.exe
) else (
    echo.
    echo Plugin built. Install Inno Setup 6 (jrsoftware.org/isinfo.php) and re-run to create Essence_Setup.exe
)
exit /b 0
:err
echo Build failed.
exit /b 1
