@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM Fast build script (vcpkg manifest mode + CMake)
REM - Configure only when needed (first run / --reconfigure / --clean)
REM - Keep incremental builds fast

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"

REM Defaults
set "CONFIG=Release"
set "TRIPLET=x64-windows"
set "GEN=Visual Studio 17 2022"
set "ARCH=x64"
set "BUILD_DIR=%ROOT%\build-vcpkg-%TRIPLET%"

REM Args:
REM   build.bat [Debug|Release] [triplet] [--clean] [--reconfigure]
REM Examples:
REM   build.bat Release x64-windows --clean
REM   build.bat Debug x86-windows --reconfigure
if not "%~1"=="" set "CONFIG=%~1"
if not "%~2"=="" (
  set "TRIPLET=%~2"
  set "BUILD_DIR=%ROOT%\build-vcpkg-%TRIPLET%"
)

set "DO_CLEAN=0"
set "DO_RECONF=0"

REM allow flags in 3rd or 4th position
for %%A in ("%~3" "%~4") do (
  if /I "%%~A"=="--clean" set "DO_CLEAN=1"
  if /I "%%~A"=="--reconfigure" set "DO_RECONF=1"
)

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

REM Clean if requested
if "%DO_CLEAN%"=="1" (
  echo [INFO] Cleaning build directory
  if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Decide whether to configure
if not exist "%BUILD_DIR%\CMakeCache.txt" set "DO_RECONF=1"
if "%DO_RECONF%"=="1" (
  echo [INFO] Configuring (CMake generate)
  cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "%GEN%" -A %ARCH% -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" -DVCPKG_TARGET_TRIPLET=%TRIPLET%
  if errorlevel 1 (
    echo.
    echo [ERROR] CMake configure failed.
    echo [HINT] Try: build.bat %CONFIG% %TRIPLET% --clean
    pause
    exit /b 1
  )
) else (
  echo [INFO] Skipping configure (incremental build)...
)

REM Build (incremental)
echo [INFO] Building...
cmake --build "%BUILD_DIR%" --config %CONFIG% --parallel

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
