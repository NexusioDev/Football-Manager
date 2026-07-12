#include <iostream>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include <filesystem>
#include <ctime>

class Team {
public:
    std::string name;
    int attack;
    int defense;
};

Team loadTeam(nlohmann::json& data, std::string name) {
    Team team;

    team.name = name;
    team.attack = data[name]["attack"];
    team.defense = data[name]["defense"];

    return team;
}

void GoalEvent(int min, const std::string& team) {
    std::cout << "Goal for " << team << " Minute: " << min << std::endl;
}

void YellowCardEvent(int min, const std::string& team) {
    std::cout << "Yellow card for " << team << " Minute: " << min << std::endl;
}

void RedCardEvent(int min, const std::string& team) {
    std::cout << "Red card for " << team << " Minute: " << min << std::endl;
}

int main() {
    srand(time(nullptr));

    auto lang = "C++";
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

    Team team1 = loadTeam(data, "PSG");
    Team team2 = loadTeam(data, "Chelsea");

    int goal1 = 0; int goal2 = 0;
    for (int m = 1; m < 91; m++) {
        int event = rand() % 10 + 1; // 10% Chance auf irgendein Event
        if (event == 1) {
            int teamInPlay = rand() % 2 + 1; //welches team hat ein event
            if (teamInPlay == 1) {
                int eventType = rand() % 10 + 1; // Was für ein event? 10%rot, 20% gelb, 70% tor
                if (eventType == 1) { // 1 Rotekarte
                    RedCardEvent(m, team1.name);
                }
                else if (eventType < 4) { //2+3 Gelbe Karte
                    YellowCardEvent(m, team1.name);
                }
                else if (eventType > 3) { // 4-10 Torchance
                    if (team1.attack > team1.defense + (rand() % 21 - 10)) {
                        goal1++;
                        GoalEvent(m, team1.name);
                    }
                }
            }
            else if (teamInPlay == 2) {
                int eventType = rand() % 10 + 1; // Was für ein event? 10%rot, 20% gelb, 70% tor
                if (eventType == 1) { // 1 Rotekarte
                    RedCardEvent(m, team2.name);
                }
                else if (eventType < 4) { //2+3 Gelbe Karte
                    YellowCardEvent(m, team2.name);
                }
                else if (eventType > 3) { // 4-10 Torchance
                    if (team2.attack > team1.defense + (rand() % 21 - 10)) {
                        goal2++;
                        GoalEvent(m, team2.name);
                    }
                }
            }
        }
    }

    std::cout << "\n" << team1.name << " Goals: " << goal1 << "\n" << team2.name << " Goals: " << goal2 << std::endl;

    std::cout << "\nDruecke Enter zum Beenden...";
    std::cin.get();

    return 0;
}