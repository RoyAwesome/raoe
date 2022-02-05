cmake_minimum_required (VERSION 3.22)

project("roae-app"
VERSION 1.0.0
LANGUAGES CXX
)

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/sdl_module.cpp"
    "src/imgui_module.cpp"
    "src/application_module.cpp"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    "include")

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    glm
    spdlog
    SDL2::SDL2-static
    flecs::flecs_static
    imgui-sdl   
    cxxopts::cxxopts
    imgui_command_palette
    raoe::engine
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    "SDL_MAIN_HANDLED"
    "HAVE_LIBC"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/)