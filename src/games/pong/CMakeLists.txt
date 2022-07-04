cmake_minimum_required (VERSION 3.22)

raoe_add_cog(
    NAME "Pong" STATIC
    CPP_SOURCE_FILES
        "src/pong.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
        glm
        Cog::ECS
        Cog::Framework
    GEARS
        "PongGear"
)
