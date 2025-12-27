cmake_minimum_required(VERSION 3.19.2)

# vcpkg integration (must be set before `project()` on first configure)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(_VCPKG_ROOT "")
    if(DEFINED ENV{VCPKG_DIR} AND NOT "$ENV{VCPKG_DIR}" STREQUAL "")
        set(_VCPKG_ROOT "$ENV{VCPKG_DIR}")
    elseif(DEFINED ENV{VCPKG_ROOT} AND NOT "$ENV{VCPKG_ROOT}" STREQUAL "")
        set(_VCPKG_ROOT "$ENV{VCPKG_ROOT}")
    elseif(EXISTS "G:/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(_VCPKG_ROOT "G:/vcpkg")
    endif()

    if(NOT _VCPKG_ROOT STREQUAL "" AND EXISTS "${_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "${_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
        message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
    endif()
endif()

project(MapleNirvana-Client LANGUAGES CXX)

#把运行目录设置为输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)

# Add compile information for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # 启用lto -flto=auto
        # 把 MINGW-根目录\libexec\gcc\x86_64-w64-mingw32\13.2.0 下的 liblto_plugin.dll,复制到 MINGW-根目录\lib\bfd-plugins
        # 去掉调试符号
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
        # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto=auto")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
        # 对本机cpu生成特定优化代码,可能存在兼容性问题
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=auto")
    endif()
endif()

# When using vcpkg, FreeType may pick up vcpkg's ZLIB which conflicts with the
# project's bundled zlibstatic (from wzlibcpp). Disable FreeType's system zlib
# to avoid duplicate symbol link errors.
if(DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg\\.cmake")
    set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(3rdparty)
add_subdirectory(MapleNirvana)
