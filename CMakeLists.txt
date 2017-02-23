cmake_minimum_required (VERSION 2.6)

# Program major and minor version number
#set (LedDisplayTool_VERSION_MAJOR 0)
#set (LedDisplayTool_VERSION_MINOR 1)

project (sdltest C)
add_executable(sdltest main.c)

# /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0
target_link_libraries (sdltest m SDL2)

