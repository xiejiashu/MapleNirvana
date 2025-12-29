@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM One-click build script (vcpkg manifest mode + CMake)

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"

REM Defaults
set "CONFIG=Release"
set "TRIPLET=x64-windows"
set "BUILD_DIR=%ROOT%\build-vcpkg-%TRIPLET%"

REM Allow: build.bat Debug
if not "%~1"=="" set "CONFIG=%~1"

REM Optional: build.bat Debug x64-windows-static
if not "%~2"=="" (
  set "TRIPLET=%~2"
  set "BUILD_DIR=%ROOT%\build-vcpkg-%TRIPLET%"
)

REM Optional: build.bat Debug x64-windows --clean
set "DO_CLEAN=0"
if /I "%~3"=="--clean" set "DO_CLEAN=1"

REM Resolve vcpkg root
set "VCPKG_ROOT_RESOLVED="

if defined VCPKG_DIR set "VCPKG_ROOT_RESOLVED=%VCPKG_DIR%"
set "VCPKG_ROOT_RESOLVED=%VCPKG_ROOT_RESOLVED:"=%"

if exist "%ROOT%\vcpkg\vcpkg.exe" set "VCPKG_ROOT_RESOLVED=%ROOT%\vcpkg"
if not defined VCPKG_ROOT_RESOLVED (
  if exist "G:\vcpkg\vcpkg.exe" set "VCPKG_ROOT_RESOLVED=G:\vcpkg"
)

if not defined VCPKG_ROOT_RESOLVED (
  echo [ERROR] vcpkg not found.
  echo   Set VCPKG_DIR env var, or place vcpkg under: %ROOT%\vcpkg
  echo.
  pause
  exit /b 1
)

set "VCPKG_FEATURE_FLAGS=manifests"
set "VCPKG_TOOLCHAIN=%VCPKG_ROOT_RESOLVED%\scripts\buildsystems\vcpkg.cmake"

if not exist "%VCPKG_TOOLCHAIN%" (
  echo [ERROR] vcpkg toolchain not found:
  echo   %VCPKG_TOOLCHAIN%
  pause
  exit /b 1
)

echo [INFO] Repo: %ROOT%
echo [INFO] Build dir: %BUILD_DIR%
echo [INFO] Config: %CONFIG%
echo [INFO] Triplet: %TRIPLET%
echo [INFO] vcpkg: %VCPKG_ROOT_RESOLVED%
echo [INFO] Toolchain: %VCPKG_TOOLCHAIN%
echo.

REM Clean cache if requested OR if platform mismatch previously (simple safe nuke)
if "%DO_CLEAN%"=="1" (
  echo [INFO] Cleaning build directory...
  if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Configure
cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_BUILD_TYPE=%CONFIG% ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" ^
  -DVCPKG_TARGET_TRIPLET=%TRIPLET%

if errorlevel 1 (
  echo.
  echo [ERROR] CMake configure failed.
  echo [HINT] Try: build.bat %CONFIG% %TRIPLET% --clean
  pause
  exit /b 1
)

REM Build
cmake --build "%BUILD_DIR%" --config %CONFIG%
if errorlevel 1 (
  echo.
  echo [ERROR] Build failed.
  pause
  exit /b 1
)

echo.
echo [OK] Build succeeded.
pause
endlocal
