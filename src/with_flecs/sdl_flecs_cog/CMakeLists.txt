cmake_minimum_required (VERSION 3.22)

project("raoe-cog-flecs-sdl"
VERSION 1.0.0
LANGUAGES CXX
)

add_library(${PROJECT_NAME}
    STATIC
    "src/sdl_module.cpp"
    "src/sdl_cog.cpp"
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
    SDL2::SDL2-static
    raoe::engine
    raoe::cog::flecs
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    "SDL_MAIN_HANDLED"
    "HAVE_LIBC"
)


add_library(raoe::cog::sdl_flecs ALIAS ${PROJECT_NAME})
