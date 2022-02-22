cmake_minimum_required (VERSION 3.22)

function(raoe_add_cog)
    set(options STATIC SHARED NO_GENERATED_COG NOT_CPP20)
    set(oneValueArgs NAME NAMESPACE)
    set(multiValueArgs CPP_SOURCE_FILES INCLUDE_DIRECTORIES GEARS COMPILE_DEFINITIONS DEPENDENCIES)

    cmake_parse_arguments(raoe_add_cog "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    #Validate the Input Arguments
    if(NOT raoe_add_cog_NAME)
        message(FATAL_ERROR "This cog must have a name")
    endif()

    if(NOT raoe_add_cog_NAMESPACE)
        set(raoe_add_cog_NAMESPACE "Global")
    endif()

    #create the target
    string(REPLACE "::" "-" COG_LIBRARY_NAME "raoe-cog-${raoe_add_cog_NAMESPACE}::${raoe_add_cog_NAME}")
    if(raoe_add_cog_STATIC)        
        add_library("${COG_LIBRARY_NAME}"
            STATIC
            ${raoe_add_cog_CPP_SOURCE_FILES}
        )        
    elseif(raoe_add_cog_SHARED)
        message(FATAL_ERROR "Not Implemented")
    else()
        message(FATAL_ERROR "Must Have Static or Shared")
    endif()

    if(NOT raoe_add_cog_NOT_CPP20)
        set_property(TARGET ${COG_LIBRARY_NAME} PROPERTY CXX_STANDARD 20)
    endif()

    set(raoe_cog_library_alias "${raoe_add_cog_NAMESPACE}::${raoe_add_cog_NAME}")
    add_library("${raoe_cog_library_alias}" ALIAS ${COG_LIBRARY_NAME})

    if(raoe_add_cog_STATIC)     
        set_property(GLOBAL APPEND PROPERTY RAOE_ALL_STATIC_COGS ${raoe_cog_library_alias})
    endif()

    #add include directories    
    target_include_directories(${COG_LIBRARY_NAME}
        ${raoe_add_cog_INCLUDE_DIRECTORIES}
    )

    #if this cog has a third_party directory, include it now
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party")
    endif()

    #add dependencies, always depending on raoe::engine
    target_link_libraries(
    ${COG_LIBRARY_NAME}
    ${raoe_add_cog_DEPENDENCIES}
    PUBLIC    
        raoe::core
        raoe::engine
    )

    #add compile definitions. 
    string(TOUPPER "${raoe_add_cog_NAME}_API" cog_api)
    target_compile_definitions(${COG_LIBRARY_NAME}
    PRIVATE
    "${cog_api}=1"
    "COG_NAME=${raoe_add_cog_NAME}"
    "COG_NAMESPACE=${raoe_add_cog_NAMESPACE}"
    ${raoe_add_cog_COMPILE_DEFINITIONS}
    )

    #generate the cog file, unless the target says no
    if(NOT raoe_add_cog_NO_GENERATED_COG)
        set(cog_generated_namespace ${raoe_add_cog_NAMESPACE})
        set(cog_generated_name ${raoe_add_cog_NAME})
        set(cog_generated_gear_external_definitions "")
        set(cog_generated_gear_external_declarations "")
        #if this has gears, generate them
        foreach(cog_gear_name ${raoe_add_cog_GEARS})
            string(APPEND cog_generated_gear_external_definitions "extern RAOE::Cogs::Gear* __GENERATED_CONSTRUCT_${cog_gear_name}();")
            string(APPEND cog_generated_gear_external_declarations "RAOE_REGISTER_GEAR_FACTORY(${cog_gear_name})")
        endforeach()
        

        set(cog_generated_external_function_name "__GENERATED__${raoe_add_cog_NAME}")
        set_property(GLOBAL APPEND PROPERTY RAOE_STATIC_COG_FUNCTION_NAMES ${cog_generated_external_function_name})

        set(cog_generated_FILE_NAME "generated_cog.cpp")
        set(cog_generated_FULL_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/${cog_generated_FILE_NAME}")
        
        configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/codegen/generated_cog.in
            ${cog_generated_FULL_FILE_PATH}
        )

        target_sources(${COG_LIBRARY_NAME}
            PRIVATE
            ${cog_generated_FULL_FILE_PATH}
        )

    endif()
endfunction()

function(raoe_generate_static_cogs targets_to_set_include)
    unset(all_static_cog_function_declarations)
    unset(all_static_cog_function_definitions)

    unset(all_static_cog_function_names)
    get_property(all_static_cog_function_names GLOBAL PROPERTY RAOE_STATIC_COG_FUNCTION_NAMES)

    foreach(static_cog_function_name ${all_static_cog_function_names})
        string(APPEND all_static_cog_function_declarations "extern \"C\" char ${static_cog_function_name}();\n")
        string(APPEND all_static_cog_function_definitions "${static_cog_function_name}();\n")
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

function(raoe_link_static_cogs_to_target target_to_add_deps_to)
    if(NOT target_to_add_deps_to)
        message(ERROR "Must have a target to set static cogs to link to")
    endif()

    raoe_generate_static_cogs(${target_to_add_deps_to})

    get_property(all_static_cogs GLOBAL PROPERTY RAOE_ALL_STATIC_COGS)

    target_link_libraries(${target_to_add_deps_to}
        PRIVATE
        ${all_static_cogs}
    )        
endfunction()