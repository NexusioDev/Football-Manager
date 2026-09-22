#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>

#include "nlohmann/json.hpp"
#include "GameEngine.hpp"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    srand(static_cast<unsigned int>(time(nullptr)));

    std::cout << "=== Football Manager 2026 ===\n";

    // Standard-Datenbank laden
    std::ifstream file("database/teams.json");
    if (!file.is_open()) {
        std::cout << "Fehler: database/teams.json nicht gefunden!\n";
        return 1;
    }
    nlohmann::json teamsData;
    file >> teamsData;
    file.close();

    // Game Engine instanziieren und neue Spielwelt laden
    GameEngine engine;
    try {
        engine.initNewGame(teamsData);
    } catch (const std::exception& e) {
        std::cout << "Fehler beim Initialisieren der Spielwelt: " << e.what() << "\n";
        return 1;
    }

    while (true) {
        engine.printCurrentDate();

        if (engine.areAllLeaguesFinished()) {
            std::cout << "\nAlle Spiele der aktuellen Saison sind absolviert!\n";
            std::cout << "Möchtest du die neue Saison starten? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                engine.startNextSeason();
            }
        }
        std::cout << "\n=== Hauptmenue ===\n"
                  << "1) Ligen & Tabellen anzeigen\n"
                  << "2) Naechsten Spieltag simulieren (Alle Ligen!)\n"
                  << "3) Spielstand speichern\n"
                  << "4) Spielstand laden\n"
                  << "5) Beenden\n"
                  << "Wahl: ";
        int choice;
        std::cin >> choice;

        if (choice == 5) break;

        if (choice == 1) {
            std::cout << "\n--- Verfuegbare Ligen ---\n";
            std::vector<std::string> keys;
            int idx = 1;
            for (const auto& [name, league] : engine.getLeagues()) {
                std::cout << idx++ << ") " << name << "\n";
                keys.push_back(name);
            }
            std::cout << "Liga waehlen: ";
            int lChoice; std::cin >> lChoice;

            if (lChoice >= 1 && lChoice <= static_cast<int>(keys.size())) {
                std::string selectedKey = keys[lChoice - 1];
                engine.getLeague(selectedKey)->printTable();
            }
        }
        else if (choice == 2) {
            // Simuliert alle Ligen parallel weiter
            engine.advanceMatchday();
        }
        else if (choice == 3) {
            try {
                engine.saveWorld("savegame.json", teamsData);
                std::cout << "Gesamte Spielwelt (alle Ligen) gespeichert!\n";
            } catch (const std::exception& e) {
                std::cout << "Fehler beim Speichern: " << e.what() << "\n";
            }
        }
        else if (choice == 4) {
            try {
                engine.loadWorld("savegame.json", teamsData);
                std::cout << "Spielwelt erfolgreich geladen!\n";
            } catch (const std::exception& e) {
                std::cout << "Fehler beim Laden: " << e.what() << "\n";
            }
        }
    }

    return 0;
}