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
    VERSION 0.1.0
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

#################################
## define bundle configuration ##
#################################

# define build type if any
IF(NOT DEFINED APP_BUNDLE_TYPE)
    IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        SET(APP_BUNDLE_TYPE WIN32) 
    ELSEIF(APPLE)
        SET(APP_BUNDLE_TYPE MACOSX_BUNDLE)
    endif()
endif()

#############
## Sources ##
#############

# library interface
add_library(itunes-library-parser INTERFACE)
target_include_directories(itunes-library-parser INTERFACE src)
target_compile_options(itunes-library-parser INTERFACE "-march=native") # for AVX2 compatibility ensured
target_compile_definitions(itunes-library-parser INTERFACE $<$<CONFIG:Debug>:_DEBUG>)

# may produce tests ?
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()