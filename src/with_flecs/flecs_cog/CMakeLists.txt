cmake_minimum_required (VERSION 3.22)

project("raoe-cog-flecs"
VERSION 1.0.0
LANGUAGES CXX
)

add_subdirectory("third_party")

add_library(${PROJECT_NAME}
    STATIC
    "src/flecs_cog.cpp"
    "src/client_app_module.cpp"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    "include"
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC    
    raoe::engine
    flecs::flecs_static
)

add_library(raoe::cog::flecs ALIAS ${PROJECT_NAME})

raoe_register_cog("flecs_cog")
