cmake_minimum_required(VERSION 3.20)

project(tomus)
set(CMAKE_CXX_STANDARD 20)

include(CPM.cmake)
CPMAddPackage("gh:raysan5/raylib#5.5")
CPMAddPackage("gh:nlohmann/json@3.10.5")


add_executable(tomus main.cpp tomus/config.cpp tomus/tomus.cpp)
target_link_libraries(tomus PUBLIC raylib nlohmann_json::nlohmann_json)
add_custom_command(
    TARGET tomus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res
)
