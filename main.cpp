#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <ctime>
#include <vector>

#include "nlohmann/json.hpp"
#include "Team.hpp"
#include "Match.hpp"

int main() {
    srand(time(nullptr));

    auto lang = "Football Manager";
    std::cout << "Hello and welcome to " << lang << "!\n";

    std::cout << "Aktueller Ordner: "
          << std::filesystem::current_path() << "\n"
          << std::endl;

    std::ifstream file("teams.json");
    if (!file.is_open()) {
        std::cout << "Fehler: teams.json nicht gefunden!" << std::endl;
        return 1;
    }
    nlohmann::json data;
    file >> data;
    file.close();

    std::vector<std::string> teamNames;

    for (auto& team : data.items()) {
        teamNames.push_back(team.key());
    }

    while (true) {
        int random1 = rand() % teamNames.size();
        int random2 = rand() % teamNames.size();

        while (random1 == random2) {
            random2 = rand() % teamNames.size();
        }

        Team team1 = loadTeam(data, teamNames[random1]);
        Team team2 = loadTeam(data, teamNames[random2]);

        Match match(team1, team2);

        match.simulate();
        match.printResult();

        std::cout << "\nDruecke Enter zum nächsten Game...";
        std::cin.get();
    }

    return 0;
}