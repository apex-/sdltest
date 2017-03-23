cmake_minimum_required (VERSION 2.6)

# Program major and minor version number
#set (LedDisplayTool_VERSION_MAJOR 0)
#set (LedDisplayTool_VERSION_MINOR 1)

project (sdltest CXX)                                                                                         
                                                                                                               
set(SOURCES                                                                                                   
    src/camera.cc
    src/main.cc 
    src/matrices.cc  
    src/plane.cc
    src/rasterizer.cc
    src/tinyobjloader.cc
    src/tlcprimitive.cc
    src/transform.cc
    src/vertex.cc 
)                                                                                                             
include_directories(include)                                                                               
add_executable(sdltest ${SOURCES})                                                                            
target_link_libraries (sdltest m SDL2)

