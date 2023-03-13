CMAKE_MINIMUM_REQUIRED(VERSION 3.18.1)
project("Opengl Project")

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Opengl ${SOURCES})

target_link_libraries(Opengl GLEW OpenGL glfw assimp stb)
