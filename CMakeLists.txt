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

add_executable(test src/main.cpp)