cmake_minimum_required (VERSION 3.22)

raoe_add_cog(
    NAME "NewCog" STATIC
    NAMESPACE "Testing"
    CPP_SOURCE_FILES
        "src/new_cog.cpp"
    INCLUDE_DIRECTORIES
        PUBLIC
            "include"
    DEPENDENCIES
        PUBLIC
            glm
            spdlog
            vc::core
)