cmake_minimum_required (VERSION 3.22)

FetchContent_Declare(
    sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG "tags/release-2.0.18"
)

if(NOT sdl2_POPULATED)
    FetchContent_MakeAvailable(sdl2)
endif()
