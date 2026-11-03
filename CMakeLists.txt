
cmake_minimum_required(VERSION 3.16)

project(SpiderHeroDemo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw")
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")

add_executable(SpiderHero
    src/main.cpp
    "${GLAD_DIR}/src/glad.c"
)

target_include_directories(SpiderHero PRIVATE
    "${GLFW_DIR}/include"
    "${GLAD_DIR}/include"
)

target_link_libraries(SpiderHero PRIVATE
    "${GLFW_DIR}/lib-vc2022/glfw3.lib"
    opengl32
)
