cmake_minimum_required (VERSION 3.22)

raoe_add_cog(
    NAME "SDL" STATIC
    CPP_SOURCE_FILES
        "src/sdl_gear.cpp"
        "src/sdl_module.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
            glm
            SDL2::SDL2-static
            Cog::ECS
    GEARS
        "SDLGear"
    COMPILE_DEFINITIONS
        "SDL_MAIN_HANDLED"
        "HAVE_LIBC"
)
