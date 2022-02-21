cmake_minimum_required (VERSION 3.22)

project("raoe"
VERSION 1.0.0
LANGUAGES CXX)

include(cmake/Cogs.cmake)

add_subdirectory("third_party")
add_subdirectory("src")

raoe_generate_static_cogs("raoe-app")
raoe_link_static_cogs_to_target("raoe-app")