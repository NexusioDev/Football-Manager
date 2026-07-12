#pragma once

#include <string>
#include "nlohmann/json.hpp"

class Team {
public:
    std::string name;
    int attack;
    int defense;
};

Team loadTeam(nlohmann::json& data, const std::string& name);