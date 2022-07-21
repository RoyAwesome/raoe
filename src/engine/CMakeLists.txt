cmake_minimum_required (VERSION 3.22)

project("raoe-engine"
VERSION 1.0.0
LANGUAGES CXX
)

add_library(${PROJECT_NAME}
    STATIC
    "src/engine.cpp"
    "src/console/console.cpp"
    "src/cogs/cog.cpp"
    "src/cogs/cog_service.cpp"
    "src/cogs/gear_service.cpp"
    "src/resource/tag.cpp"
    "src/resource/service.cpp"
    "src/resource/handle.cpp"
    "src/resource/type.cpp"
    "src/services/task_service.cpp"
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
    spdlog
    raoe::core
    ctre::ctre
)

add_library(raoe::engine ALIAS ${PROJECT_NAME})

add_subdirectory("test")
add_subdirectory("third_party")
