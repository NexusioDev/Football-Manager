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
        Cup.cpp
        Cup.hpp
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/database/teams.json
        ${CMAKE_CURRENT_BINARY_DIR}/database/teams.json
        COPYONLY
)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/database/league.json
        ${CMAKE_CURRENT_BINARY_DIR}/database/league.json
        COPYONLY
)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/database/cup.json
        ${CMAKE_CURRENT_BINARY_DIR}/database/cup.json
        COPYONLY
)