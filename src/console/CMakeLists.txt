cmake_minimum_required (VERSION 3.22)

raoe_add_cog(
    NAME "Console" STATIC
    CPP_SOURCE_FILES
        "src/console_gear.cpp"
        "src/display_console.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
            imgui
    GEARS
        "ConsoleGear"
)
