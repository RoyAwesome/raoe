cmake_minimum_required (VERSION 3.22)

project("raoe-cog-imgui-sdl-flecs"
VERSION 1.0.0
LANGUAGES CXX
)

add_subdirectory("third_party")

add_library(${PROJECT_NAME}
    STATIC
    "src/imgui_module.cpp"
    "src/imgui_cog.cpp"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    "include"
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    glm
    imgui-sdl  
    imgui_command_palette 
    raoe::engine
    raoe::cog::flecs
    raoe::cog::sdl_flecs
)

add_library(raoe::cog::imgui_sdl_flecs ALIAS ${PROJECT_NAME})

raoe_register_cog("imgui_sdl_flecs_cog")
