#include "Team.hpp"

Team loadTeam(nlohmann::json& data, const std::string& name) {
    Team team;

    team.name = name;
    team.attack = data[name]["attack"].get<int>();
    team.defense = data[name]["defense"].get<int>();

    return team;
}