cmake_minimum_required (VERSION 3.22)

#Download CPM
set(CPM_DOWNLOAD_VERSION 0.34.3)

if(CPM_SOURCE_CACHE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD
       https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION}
  )
endif()
include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage("gh:SanderMertens/flecs#master")
CPMAddPackage("gh:g-truc/glm#master")
CPMAddPackage("gh:gabime/spdlog#v1.9.2")
CPMAddPackage("gh:RoyAwesome/vccore#main")

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

    add_library(imgui
        STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
    )

    target_include_directories(imgui
        PUBLIC
        ${imgui_SOURCE_DIR}
    )

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

CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  GIT_TAG "tags/v3.0.0"
  OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
)

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