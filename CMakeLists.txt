# use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message("== Using CCache ! ==")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# minimal required
cmake_minimum_required(VERSION 3.19.3 FATAL_ERROR)

# project definition
project(ITunesLibraryParser
    VERSION 0.0.1
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

################################
## CPP Compiler Configuration ##
################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

#############
## Sources ##
#############

# library interface
add_library(itunes-library-parser INTERFACE)
target_include_directories(itunes-library-parser INTERFACE src)
target_compile_options(itunes-library-parser INTERFACE "-march=native")

# test executable
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    #
    add_executable(${PROJECT_NAME}_test 
        test/main.cpp
    )
    
    # LTO on, reduces exe size
    set_target_properties(${PROJECT_NAME}_test PROPERTIES 
        INTERPROCEDURAL_OPTIMIZATION ON
    )
    
    # link
    target_link_libraries(${PROJECT_NAME}_test PRIVATE
        itunes-library-parser
    )
endif()