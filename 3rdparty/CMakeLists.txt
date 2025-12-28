add_subdirectory(freetype)

set(SDL_SHARED FALSE)
set(SDL_STATIC TRUE)
add_subdirectory(SDL)

# ---------------------------------------------------------------------------
# Third-party configuration
#
# Keep dependencies lean: don't build their tests/benchmarks by default.
# Some submodules call find_package(GTest REQUIRED) inside their test trees.
# ---------------------------------------------------------------------------
include(CTest) # defines BUILD_TESTING option
set(BUILD_TESTING OFF CACHE BOOL "Disable tests for third-party dependencies" FORCE)
set(CMAKE_DISABLE_FIND_PACKAGE_GTest TRUE CACHE BOOL "Disable GTest discovery in third-party" FORCE)

add_subdirectory(entt)

# ---------------------------------------------------------------------------
# AES
#
# Upstream 3rdparty/AES CMake builds only a GTest-based "tests" executable and
# requires a very old cmake_minimum_required(). That breaks modern CMake and
# vcpkg toolchains on Windows.
#
# We don't modify the submodule; we compile the library directly.
# ---------------------------------------------------------------------------
add_library(aes STATIC
	${CMAKE_CURRENT_LIST_DIR}/AES/AES.cpp
)
add_library(AES::AES ALIAS aes)
target_include_directories(aes PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(zlib)
add_subdirectory(mio)
