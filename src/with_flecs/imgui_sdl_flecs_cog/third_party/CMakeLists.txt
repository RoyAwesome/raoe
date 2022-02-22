cmake_minimum_required (VERSION 3.22)

FetchContent_Declare(
    sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG "tags/release-2.0.18"
)

if(NOT sdl2_POPULATED)
    FetchContent_MakeAvailable(sdl2)
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG "origin/master"
)
if(NOT imgui_POPULATED)
  FetchContent_MakeAvailable(imgui)
    add_library(imgui-sdl
        STATIC
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer.cpp"
    )
    target_include_directories(imgui-sdl
        PUBLIC
        "${imgui_SOURCE_DIR}/backends/"
    )
    target_link_libraries(imgui-sdl
        PUBLIC
        imgui
        SDL2::SDL2-static
    )
endif()


FetchContent_Declare(
    imgui_command_palette
    GIT_REPOSITORY "https://github.com/hnOsmium0001/imgui-command-palette"
    GIT_TAG "origin/master"
)

if(NOT imgui_command_palette_POPULATED)
  FetchContent_MakeAvailable(imgui_command_palette)

  add_library(imgui_command_palette
        STATIC
        "${imgui_command_palette_SOURCE_DIR}/imcmd_command_palette.cpp"
        "${imgui_command_palette_SOURCE_DIR}/imcmd_fuzzy_search.cpp"
)

    target_include_directories(imgui_command_palette
        PUBLIC
        ${imgui_command_palette_SOURCE_DIR}
    )

    target_link_libraries(imgui_command_palette
        PUBLIC
        imgui
    )

endif()