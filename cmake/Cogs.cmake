cmake_minimum_required (VERSION 3.22)




function(raoe_register_cog)

    set(one_value_args COG_NAME)

    cmake_parse_arguments(raoe_register_cog "${one_value_args}" ${ARGN})

    set_property(GLOBAL APPEND PROPERTY RAOE_STATIC_COGS ${raoe_register_cog_COG_NAME})

endfunction()