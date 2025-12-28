add_subdirectory(freetype)

set(SDL_SHARED FALSE)
set(SDL_STATIC TRUE)
add_subdirectory(SDL)

# --- disable third-party tests / gtest lookups ---
include(CTest)                 # defines BUILD_TESTING option
set(BUILD_TESTING OFF CACHE BOOL "Disable tests for third-party deps" FORCE)
set(CMAKE_DISABLE_FIND_PACKAGE_GTest TRUE CACHE BOOL "Don't try to find GTest" FORCE)
# -----------------------------------------------

add_subdirectory(entt)

# If AES has its own toggle, you can also force it here (optional; harmless if unused)
set(AES_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(AES_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(AES)
add_subdirectory(zlib)
add_subdirectory(mio)
