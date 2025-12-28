include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---------------------------------------------------------------------------
# wzlibcpp (embedded)
#
# Many game sources include headers as "wz/...". Provide a normal static
# library target and a stable include root at `wzlibcpp/include`.
# ---------------------------------------------------------------------------
set(WZLIBCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wzlibcpp)

file(GLOB_RECURSE WZLIBCPP_SOURCES CONFIGURE_DEPENDS
    ${WZLIBCPP_DIR}/*.cpp
)

add_library(wzlib STATIC ${WZLIBCPP_SOURCES})
target_include_directories(wzlib PUBLIC
    ${WZLIBCPP_DIR}
    ${WZLIBCPP_DIR}/include
)
target_link_libraries(wzlib PUBLIC
    AES::AES
    mio::mio
    ZLIB::ZLIBSTATIC
)

set(_default_ffmpeg ON)
if(EMSCRIPTEN OR ANDROID)
    set(_default_ffmpeg OFF)
endif()
option(MAPLENIRVANA_USE_FFMPEG "Enable FFmpeg support" ${_default_ffmpeg})

set(_default_lua ON)
if(EMSCRIPTEN OR ANDROID)
    set(_default_lua OFF)
endif()
option(MAPLENIRVANA_USE_LUA "Enable embedded Lua scripting (Lua 5.4 via vcpkg)" ${_default_lua})

function(maplenirvana_link_lua target_name)
    if(NOT MAPLENIRVANA_USE_LUA)
        return()
    endif()

    # Prefer CMake's built-in FindLua module. With vcpkg toolchain enabled,
    # this will pick up the vcpkg-provided Lua.
    find_package(Lua QUIET)
    if(LUA_FOUND)
        # FindLua doesn't guarantee an imported target.
        if(DEFINED LUA_INCLUDE_DIR)
            target_include_directories(${target_name} PRIVATE ${LUA_INCLUDE_DIR})
        endif()
        if(DEFINED LUA_LIBRARIES)
            target_link_libraries(${target_name} PRIVATE ${LUA_LIBRARIES})
        endif()
        return()
    endif()

    # Fallback for environments that export a config package instead.
    find_package(unofficial-lua CONFIG QUIET)
    if(unofficial-lua_FOUND)
        if(TARGET unofficial::lua::lua)
            target_link_libraries(${target_name} PRIVATE unofficial::lua::lua)
            return()
        elseif(TARGET unofficial::lua)
            target_link_libraries(${target_name} PRIVATE unofficial::lua)
            return()
        endif()
        message(FATAL_ERROR "Found unofficial-lua but no known targets were exported")
    endif()

    message(FATAL_ERROR "Lua requested but not found. Install via vcpkg (lua) or disable MAPLENIRVANA_USE_LUA")
endfunction()

function(maplenirvana_link_ffmpeg target_name)
    if(NOT MAPLENIRVANA_USE_FFMPEG)
        return()
    endif()

    # vcpkg's ffmpeg port provides a Find module that defines FFmpeg:: targets.
    find_package(FFMPEG REQUIRED)

    if(TARGET FFmpeg::avcodec)
        target_link_libraries(${target_name} PRIVATE
            FFmpeg::avformat
            FFmpeg::avcodec
            FFmpeg::avutil
            FFmpeg::swscale
            FFmpeg::swresample
        )

        if(TARGET FFmpeg::avdevice)
            target_link_libraries(${target_name} PRIVATE FFmpeg::avdevice)
        endif()
        if(TARGET FFmpeg::avfilter)
            target_link_libraries(${target_name} PRIVATE FFmpeg::avfilter)
        endif()
    else()
        # Fallback for non-target FindFFMPEG implementations
        target_include_directories(${target_name} PRIVATE ${FFMPEG_INCLUDE_DIRS})
        target_link_libraries(${target_name} PRIVATE ${FFMPEG_LIBRARIES})
    endif()
endfunction()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# `main.cpp` is added explicitly per-platform.
list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# wzlibcpp sources live in the `wzlib` target.
list(REMOVE_ITEM SOURCE_FILES ${WZLIBCPP_SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${WZLIBCPP_SOURCES})

if(WIN32)
    add_executable(MapleNirvana ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE_FILES})
    # Link SDL3
    target_link_libraries(MapleNirvana PRIVATE SDL3::SDL3)
    # Link EnTT
    target_link_libraries(MapleNirvana PRIVATE EnTT::EnTT)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(MapleNirvana PRIVATE /utf-8)
    endif()

    # 链接 wzlib 库
    target_link_libraries(MapleNirvana PRIVATE wzlib)
    # FFmpeg
    maplenirvana_link_ffmpeg(MapleNirvana)
    # Lua
    maplenirvana_link_lua(MapleNirvana)
    # freetype
    target_link_libraries(MapleNirvana PRIVATE freetype)

    # Lua compile-time flag (0/1)
    target_compile_definitions(MapleNirvana PRIVATE MAPLENIRVANA_USE_LUA=$<BOOL:${MAPLENIRVANA_USE_LUA}>)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_definitions(-DNOMINMAX)
    endif()

    set_target_properties(MapleNirvana PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # 工作目录设置为输出目录
    set_target_properties(MapleNirvana PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Debug
    )
endif()

if(LINUX)
    add_executable(MapleNirvana ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE_FILES})
    # Link SDL3
        target_link_libraries(MapleNirvana PRIVATE SDL3::SDL3)
        # Link EnTT
        target_link_libraries(MapleNirvana PRIVATE EnTT::EnTT)
        # 链接 wzlib 库
        target_link_libraries(MapleNirvana PRIVATE wzlib)
        # FFmpeg
        maplenirvana_link_ffmpeg(MapleNirvana)
        # Lua
        maplenirvana_link_lua(MapleNirvana)
        # freetype
        target_link_libraries(MapleNirvana PRIVATE freetype)
        # Lua compile-time flag (0/1)
        target_compile_definitions(MapleNirvana PRIVATE MAPLENIRVANA_USE_LUA=$<BOOL:${MAPLENIRVANA_USE_LUA}>)
        set_target_properties(MapleNirvana PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
endif()

# 设置 Emscripten 编译选项
if(EMSCRIPTEN)
    add_executable(MapleNirvana ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE_FILES})
    # Link SDL3
    target_link_libraries(MapleNirvana PRIVATE SDL3::SDL3)
    # Link EnTT
    target_link_libraries(MapleNirvana PRIVATE EnTT::EnTT)
    # 链接 wzlib 库
    target_link_libraries(MapleNirvana PRIVATE wzlib)
    # FFmpeg (default OFF on Emscripten)
    maplenirvana_link_ffmpeg(MapleNirvana)
    # Lua (default OFF on Emscripten)
    maplenirvana_link_lua(MapleNirvana)
    # freetype
    target_link_libraries(MapleNirvana PRIVATE freetype)

    # Lua compile-time flag (0/1)
    target_compile_definitions(MapleNirvana PRIVATE MAPLENIRVANA_USE_LUA=$<BOOL:${MAPLENIRVANA_USE_LUA}>)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
        set_target_properties(MapleNirvana PROPERTIES LINK_FLAGS
            "--preload-file ${CMAKE_SOURCE_DIR}/build/Data/@Data/"
            # 可以添加其他Emscripten链接器选项
        )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sALLOW_MEMORY_GROWTH -g -sUSE_ZLIB")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sALLOW_MEMORY_GROWTH -g -sUSE_ZLIB")
        set_target_properties(MapleNirvana PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
endif()

if(ANDROID)
    add_library(main SHARED)
    target_sources(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE_FILES})
    target_link_libraries(main PRIVATE SDL3::SDL3 android)
    target_link_libraries(main PRIVATE EnTT::EnTT)
    # 链接 wzlib 库
    target_link_libraries(main wzlib)
    # FFmpeg (default OFF on Android)
    maplenirvana_link_ffmpeg(main)
    # Lua (default OFF on Android)
    maplenirvana_link_lua(main)
    # freetype
    target_link_libraries(main freetype)

    # Lua compile-time flag (0/1)
    target_compile_definitions(main PRIVATE MAPLENIRVANA_USE_LUA=$<BOOL:${MAPLENIRVANA_USE_LUA}>)
endif()

if (APPLE)
    # Find packages
    find_library(COREMEDIA_FRAMEWORK CoreMedia)
    find_library(VIDEOTOOLBOX_FRAMEWORK VideoToolbox)
    find_library(SECURITY_FRAMEWORK Security)

    add_executable(MapleNirvana ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCE_FILES})

    # Link SDL3
    target_link_libraries(MapleNirvana PRIVATE SDL3::SDL3)
    # Link EnTT
    target_link_libraries(MapleNirvana PRIVATE EnTT::EnTT)

    # 链接 wzlib 库
    target_link_libraries(MapleNirvana PRIVATE wzlib)
    # FFmpeg
    maplenirvana_link_ffmpeg(MapleNirvana)
    # Lua
    maplenirvana_link_lua(MapleNirvana)
    # freetype
    target_link_libraries(MapleNirvana PRIVATE freetype)

    # Lua compile-time flag (0/1)
    target_compile_definitions(MapleNirvana PRIVATE MAPLENIRVANA_USE_LUA=$<BOOL:${MAPLENIRVANA_USE_LUA}>)

    # Link frameworks
    target_link_libraries(
        MapleNirvana
        PRIVATE
        ${COREMEDIA_FRAMEWORK}
        ${VIDEOTOOLBOX_FRAMEWORK}
        ${SECURITY_FRAMEWORK}
    )

    set_target_properties(MapleNirvana PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()
