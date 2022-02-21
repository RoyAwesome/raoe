cmake_minimum_required (VERSION 3.22)

raoe_add_cog(
    NAME "ECS" STATIC
    NAMESPACE "Flecs"
    CPP_SOURCE_FILES
        "src/flecs_gear.cpp"
        "src/client_app_module.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
            flecs::flecs_static
    GEARS
        "FlecsGear"
)
