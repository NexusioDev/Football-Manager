cmake_minimum_required(VERSION 4.1)
project(Football_Manager)

set(CMAKE_CXX_STANDARD 20)

include_directories(nlohmann)

add_executable(Football_Manager
        nlohmann/json.hpp
        main.cpp
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/teams.json
        ${CMAKE_CURRENT_BINARY_DIR}/teams.json
        COPYONLY
)