CMAKE_MINIMUM_REQUIRED(VERSION 3.18.1)
project("Opengl Project")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Opengl ${SOURCES})
# add_executable(Tester ${CMAKE_CURRENT_SOURCE_DIR}/Test.cpp)

target_link_libraries(Opengl GLEW OpenGL glfw assimp glm)
# target_link_libraries(Tester glm)
