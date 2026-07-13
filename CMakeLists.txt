cmake_minimum_required(VERSION 4.1)
project(Football_Manager)

set(CMAKE_CXX_STANDARD 20)

include_directories(nlohmann)

add_executable(Football_Manager
        nlohmann/json.hpp
        main.cpp
        Match.cpp
        Match.hpp
        Team.cpp
        Team.hpp
        Events.cpp
        Events.hpp
        League.cpp
        League.hpp
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/teams.json
        ${CMAKE_CURRENT_BINARY_DIR}/teams.json
        COPYONLY
)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/league.json
        ${CMAKE_CURRENT_BINARY_DIR}/league.json
        COPYONLY
)