cmake_minimum_required(VERSION 3.16)
project(SpiderHero LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(SpiderHero src/main.cpp external/glad/src/glad.c)
target_include_directories(SpiderHero PRIVATE external/glad/include external/glfw/include)
if(WIN32)
  target_link_libraries(SpiderHero PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/lib-vc2022/glfw3.lib"
    opengl32 winmm)
else()
  find_package(glfw3 REQUIRED)
  target_link_libraries(SpiderHero PRIVATE glfw GL dl pthread)
endif()
add_custom_command(TARGET SpiderHero POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:SpiderHero>/shaders"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    "$<TARGET_FILE_DIR:SpiderHero>/shaders")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/song.mp3")
  add_custom_command(TARGET SpiderHero POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/song.mp3"
      "$<TARGET_FILE_DIR:SpiderHero>/song.mp3")
endif()
