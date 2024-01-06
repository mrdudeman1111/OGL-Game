CMAKE_MINIMUM_REQUIRED(VERSION 3.18.1)
project("Opengl Project")

find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(Sail CONFIG REQUIRED)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_definitions(SourceDir="${CMAKE_SOURCE_DIR}")

add_executable(Game ${SOURCES})

target_link_libraries(Game glfw opengl::opengl glm::glm GLEW::GLEW assimp::assimp SAIL::SailC++)

