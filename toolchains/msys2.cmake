if(NOT DEFINED ENV{MINGW64_ROOT})
    message(FATAL_ERROR "Required ENV variable MINGW64_ROOT does not exists. Please check README.md for more details !")
else()
    SET(MINGW64_ROOT $ENV{MINGW64_ROOT})
endif()

# search for programs in the build host directories
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)

# for libraries and headers in the target directories
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

list(APPEND CMAKE_PREFIX_PATH 
    ${MINGW64_ROOT} 
)

SET(CMAKE_BUILD_TYPE Debug)