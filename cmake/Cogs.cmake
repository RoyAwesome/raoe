cmake_minimum_required (VERSION 3.22)

function(raoe_register_cog cog_name)   
    set_property(GLOBAL APPEND PROPERTY RAOE_STATIC_COGS ${cog_name})
endfunction()

function(raoe_generate_static_cogs targets_to_set_include)
    unset(extern_cog_declarations)
    unset(extern_cog_definitions)

    unset(all_static_cogs)
    get_property(all_static_cogs GLOBAL PROPERTY RAOE_STATIC_COGS)

    foreach(cog_name ${all_static_cogs})
        string(APPEND extern_cog_declarations   "extern \"C\" void __GENERATED__${cog_name}();\n")
        string(APPEND extern_cog_definitions    "__GENERATED__${cog_name}();\n")
    endforeach()

    configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/codegen/static_cogs.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/application/include/static_cogs.inl)

    if(targets_to_set_include)
       foreach(cog_include_target ${targets_to_set_include})
            target_include_directories(${cog_include_target}
                PRIVATE
                ${CMAKE_CURRENT_BINARY_DIR}/src/application/include/
            )
       endforeach()
    endif()
endfunction()   