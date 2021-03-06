# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.19)


include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG "origin/main"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

project("core-test")

enable_testing()

add_executable(${PROJECT_NAME}
    "parse_test.cpp"
#    "coro_test.cpp"
    "command_func_test.cpp"
    "string_test.cpp"
    "subclass_map_test.cpp"
    "lazy_test.cpp"
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_definitions(${PROJECT_NAME} PRIVATE
                            SEQUENTIAL_PROCESSING=0)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    gtest_main
    gmock_main
    raoe::core
)
