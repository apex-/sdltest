cmake_minimum_required (VERSION 2.6)

# Program major and minor version number
#set (LedDisplayTool_VERSION_MAJOR 0)
#set (LedDisplayTool_VERSION_MINOR 1)

project (sdltest CXX)                                                                                         
                                                                                                               
set(SOURCES                                                                                                   
    src/Camera.cpp
    src/main.cpp 
    src/Matrices.cpp  
    src/Rasterizer.cpp
    src/Transform.cpp
    src/TLCPrimitive.cpp
    src/tiny_obj_loader.cpp
    src/Vertex.cpp 
)                                                                                                             
include_directories(include)                                                                               
add_executable(sdltest ${SOURCES})                                                                            
target_link_libraries (sdltest m SDL2)

