# CMAKE definition file for Max
# depends on max-sdk-base to be present
#
# mkdir build && cd build
# cmake .. -Wno-dev
# make

cmake_minimum_required(VERSION 3.0)
project(xsample)

set(CMAKE_OSX_ARCHITECTURES x86_64;arm64)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# output to maxmsp folder
set(C74_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/maxmsp)

set(C74_MAX_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/max-sdk-base)
set(FLEXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flext)

include(${C74_MAX_API_DIR}/script/max-pretarget.cmake)

include_directories(
        "${C74_MAX_API_DIR}/c74support/max-includes"
        "${C74_MAX_API_DIR}/c74support/msp-includes"
        "${C74_MAX_API_DIR}/c74support/jit-includes"
        "${FLEXT_DIR}/source"
)

add_definitions(
        -DFLEXT_INLINE=1
        -DFLEXT_ATTRIBUTES=1
        -DFLEXT_SYS=1
        -DMSP64=1
)

set( SOURCE_FILES ${SOURCES} )

add_library(
        ${PROJECT_NAME}
        MODULE
        ${SOURCE_FILES}
)

include(${C74_MAX_API_DIR}/script/max-posttarget.cmake)
