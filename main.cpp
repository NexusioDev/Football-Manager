#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <ctime>
#include <vector>
#include <windows.h>

#include "nlohmann/json.hpp"
#include "Team.hpp"
#include "Match.hpp"
#include "league.hpp"
#include "Cup.hpp"

League loadLeague(nlohmann::json& teamsData, const std::string& leaguePath) {
    std::ifstream file(leaguePath);
    if (!file.is_open()) {
        throw std::runtime_error("league.json nicht gefunden!");
    }
    nlohmann::json leagueData;
    file >> leagueData;

    std::vector<std::string> leagueNames;

    for (auto& league : leagueData.items()) {
        leagueNames.push_back(league.key());
    }

    std::cout << "\n=== Ligen ===\n";
    for (size_t i = 0; i < leagueNames.size(); i++) {
        std::cout << i + 1 << ") " << leagueNames[i] << '\n';
    }

    std::cout << "Liga wählen: ";
    int choice;
    std::cin >> choice;

    if (choice < 1 || choice > leagueNames.size()) {
        throw std::runtime_error("Ungültige Auswahl!");
    }

    std::string selectedLeague = leagueNames[choice - 1];

    // Sicherheitscheck: existieren alle Teams in teams.json?
    for (const auto& name : leagueData[selectedLeague]["teams"]) {
        std::string t = name.get<std::string>();
        if (!teamsData.contains(t)) {
            throw std::runtime_error("Team \"" + t + "\" (Liga \"" + selectedLeague + "\") fehlt in teams.json – Tippfehler?");
        }
    }

    std::vector<Team> teams;
    for (const auto& name : leagueData[selectedLeague]["teams"]) {
        teams.push_back(loadTeam(teamsData, name.get<std::string>()));
    }

    int amountRelegationTeams = leagueData[selectedLeague]["amountRelegationTeams"];
    int amountRelPlayoffTeams = leagueData[selectedLeague].value("amountRelegationPlayoffTeams", 0);
    int amountCl = leagueData[selectedLeague].value("amountChampionsLeagueTeams", 0);
    int amountEl = leagueData[selectedLeague].value("amountEuropaLeagueTeams", 0);
    int amountCfl = leagueData[selectedLeague].value("amountConferenceLeagueTeams", 0);

    return League(teams, amountRelegationTeams, amountRelPlayoffTeams, amountCl, amountEl, amountCfl);
}

void runLeagueMode(nlohmann::json& teamsData) {
    League league = loadLeague(teamsData, "database/league.json");

    std::cout << "\n1) Ganze Saison simulieren\n2) Spiel für Spiel\n3) Zurück\nChoice: ";
    int mode; std::cin >> mode;

    if (mode == 1) {
        league.simulateAll();
        league.printTableFixtures();
        league.printTable();
    } else if (mode == 2) {
        while (!league.isFinished()) {
            league.simulateNextFixture();
            league.printTable();
            std::cout << "\nWeiter mit Enter...";
            std::cin.ignore(); std::cin.get();
        }
    }
}

Cup loadCup(nlohmann::json& teamsData, const std::string& cupPath) {
    std::ifstream file(cupPath);
    if (!file.is_open()) {
        throw std::runtime_error("cup.json nicht gefunden!");
    }
    nlohmann::json cupData;
    file >> cupData;

    std::vector<std::string> cupNames;

    for (auto& league : cupData.items()) {
        cupNames.push_back(league.key());
    }

    std::cout << "\n=== Pokale ===\n";
    for (size_t i = 0; i < cupNames.size(); i++) {
        std::cout << i + 1 << ") " << cupNames[i] << '\n';
    }

    std::cout << "Pokal wählen: ";
    int choice;
    std::cin >> choice;

    if (choice < 1 || choice > cupNames.size()) {
        throw std::runtime_error("Ungültige Auswahl!");
    }

    std::string selectedLeague = cupNames[choice - 1];

    std::vector<Team> teams;
    for (const auto& name : cupData[selectedLeague]["teams"]) {
        teams.push_back(loadTeam(teamsData, name.get<std::string>()));
    }

    return Cup(teams);
}

void runCupMode(nlohmann::json& teamsData) {
    Cup cup = loadCup(teamsData, "database/cup.json");

    std::cout << "\n1) Ganzen Pokal simulieren\n2) Spiel für Spiel\n3) Zurück\nChoice: ";
    int mode; std::cin >> mode;

    if (mode == 1) {
        cup.simulateAll();
        cup.printCupFixtures();
        //cup.printTable();
    } else if (mode == 2) {
        while (!cup.isFinished()) {
            cup.simulateNextFixture();
            //cup.printTable();
            std::cout << "\nWeiter mit Enter...";
            std::cin.ignore(); std::cin.get();
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    srand(time(nullptr));

    auto lang = "Football Manager";
    std::cout << "Hello and welcome to " << lang << "!\n";

    std::cout << "Aktueller Ordner: "
          << std::filesystem::current_path() << "\n"
          << std::endl;

    std::ifstream file("database/teams.json");
    if (!file.is_open()) {
        std::cout << "Fehler: teams.json nicht gefunden!" << std::endl;
        return 1;
    }
    nlohmann::json data;
    file >> data;
    file.close();

    std::vector<std::string> teamNames;

    for (auto& team : data.items()) {
        if (team.key().rfind("_", 0) == 0) {
            continue;
        }
        teamNames.push_back(team.key());
    }

    while (true) {
        std::cout << "\n=== Football Manager ===\n"
        << "1) Quick Match (random teams)\n"
        << "2) Custom Match (pick teams)\n"
        << "3) League Mode\n"
        << "4) Cup Mode\n"
        << "5) Exit\n"
        << "Choice: ";
        int choice; std::cin >> choice;
        if (choice == 5) break;
        if (choice == 1) {
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
        }
        else if (choice == 2) {
            std::cout << "\n===== Teams List =====\n";
            for (int i = 0; i < teamNames.size(); i++) {
                std::cout << i + 1 << ". " << loadTeam(data, teamNames[i]).name << "\n";
            }
            std::cout << "Team Nr.1 (Eine Zahl zwischen 1 bis " << teamNames.size() << ": ";
            int teamNo1; std::cin >> teamNo1; teamNo1--;
            std::cout << "Team Nr.2 (Eine Zahl zwischen 1 bis " << teamNames.size() << ": ";
            int teamNo2; std::cin >> teamNo2; teamNo2--;

            while (teamNo1 == teamNo2) {
                std::cout << "Selected the Same Team! A random Team will be chosen";
                teamNo2 = rand() % teamNames.size();
            }

            Team team1 = loadTeam(data, teamNames[teamNo1]);
            Team team2 = loadTeam(data, teamNames[teamNo2]);

            Match match(team1, team2);

            match.simulate();
            match.printResult();
        }
        else if (choice == 3) {
            runLeagueMode(data);
        }
        else if (choice == 4) {
            runCupMode(data);
        }
    }

    return 0;
}