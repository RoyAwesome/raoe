cmake_minimum_required (VERSION 3.22)

raoe_add_cog(
    NAME "Framework" STATIC
    CPP_SOURCE_FILES
        "src/framework_gear.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
        glm
    GEARS
        "FrameworkGear"
)
