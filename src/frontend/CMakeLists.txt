
raoe_add_cog(
    NAME "Frontend" STATIC
    CPP_SOURCE_FILES
        "src/frontend.cpp"
        "src/gear.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
        glm
        imgui #For now, we'll do the frontend in imgui
        Cog::ECS
        Cog::Console
        Cog::Framework
    GEARS
        "FrontendGear"
)
