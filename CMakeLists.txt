##------------------------------------------------------------------------------
## Vapour CMake Preamble
##

cmake_minimum_required(VERSION 3.5)
project(Vapour C)

##------------------------------------------------------------------------------
## Options
##

option(ENABLE_CLI "Compile the CLI vapour binary" ON)

##------------------------------------------------------------------------------
## External Libraries
##

find_package(CURL REQUIRED)

##------------------------------------------------------------------------------
## Sources
##

# Sources for libvapour.so
set(LIBVAPOUR_SOURCES
  src/libvapour/appinfo.c
  src/libvapour/dir.c
  src/libvapour/lib.c
  src/libvapour/steamcmd.c)

# Sources for vapour
set(VAPOUR_CLI_SOURCES
  src/vapour-cli/main.c)

##------------------------------------------------------------------------------
## Build target
##

include_directories(include)
add_compile_options(-D_GNU_SOURCE)

# libvapour.so
add_library(libvapour SHARED ${LIBVAPOUR_SOURCES})
target_include_directories(libvapour PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(libvapour ${CURL_LIBRARY})

# vapour
if (ENABLE_CLI)
  add_executable(vapour ${VAPOUR_CLI_SOURCES})
  target_link_libraries(vapour libvapour)
endif (ENABLE_CLI)