cmake_minimum_required (VERSION 3.22)


raoe_add_cog(
    NAME "Imgui_SDL" STATIC
    CPP_SOURCE_FILES
        "src/imgui_module.cpp"
        "src/imgui_cog.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
            glm
            imgui-sdl  
            imgui_command_palette 
            Cog::ECS
            Cog::SDL
            Cog::Console
    GEARS
        "ImguiGear"
)


