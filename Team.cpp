#include "Team.hpp"

Team loadTeam(nlohmann::json& data, const std::string& name) {
    if (!data.contains(name)) {
        throw std::runtime_error("Team \"" + name + "\" nicht in teams.json gefunden (Tippfehler?)");
    }

    const nlohmann::json& t = data.at(name);
    Team team;

    team.name = name;
    team.attack = t["attack"].get<int>();
    team.defense = t["defense"].get<int>();

    return team;
}