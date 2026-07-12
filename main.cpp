#include <iostream>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include <filesystem>
#include <ctime>
#include <vector>

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
    std::cout << "Goal for " << team << " - Minute: " << min << std::endl;
}

void YellowCardEvent(int min, const std::string& team) {
    std::cout << "Yellow card for " << team << " - Minute: " << min << std::endl;
}

void RedCardEvent(int min, const std::string& team) {
    std::cout << "Red card for " << team << " - Minute: " << min << std::endl;
}

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

    int random1 = rand() % teamNames.size();
    int random2 = rand() % teamNames.size();

    while (random1 == random2) {
        random2 = rand() % teamNames.size();
    }

    Team team1 = loadTeam(data, teamNames[random1]);
    Team team2 = loadTeam(data, teamNames[random2]);
    int players1 = 11; int players2 = 11; int yellows1 = 0; int yellows2 = 0;

    int goal1 = 0; int goal2 = 0;
    for (int m = 1; m < 91; m++) {
        int event = rand() % 10 + 1; // 10% Chance auf irgendein Event
        if (event == 1) {
            int teamInPlay = rand() % 2 + 1; //welches team hat ein event
            if (teamInPlay == 1) {
                int eventType = rand() % 10 + 1; // Was für ein event? 10%rot, 20% gelb, 70% tor
                if (eventType == 1) { // 1 Rotekarte
                    players1--;
                    RedCardEvent(m, team1.name);
                }
                else if (eventType < 4) { //2+3 Gelbe Karte
                    yellows1++;
                    if (yellows1 == 2) {
                        players1--;
                        yellows1 = 0;
                    }
                    YellowCardEvent(m, team1.name);
                }
                else if (eventType > 3) { // 4-10 Torchance
                    if (team1.attack * players1 / 11 > team2.defense * players2 / 11 + (rand() % 31 - 15)) {
                        goal1++;
                        GoalEvent(m, team1.name);
                    }
                }
            }
            else if (teamInPlay == 2) {
                int eventType = rand() % 10 + 1; // Was für ein event? 10%rot, 20% gelb, 70% tor
                if (eventType == 1) { // 1 Rotekarte
                    players2--;
                    RedCardEvent(m, team2.name);
                }
                else if (eventType < 4) { //2+3 Gelbe Karte
                    yellows2++;
                    if (yellows2 == 2) {
                        players2--;
                        yellows2 = 0;
                    }
                    YellowCardEvent(m, team2.name);
                }
                else if (eventType > 3) { // 4-10 Torchance
                    if (team2.attack * players2 / 11 > team1.defense * players1 / 11 + (rand() % 31 - 15)) {
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