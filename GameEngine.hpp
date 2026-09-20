#pragma once

#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"
#include "Team.hpp"
#include "League.hpp"
#include "Cup.hpp"

class GameEngine {
public:
    GameEngine() = default;

    // Initialisiert die Welt neu aus den JSON-Datenbanken
    void initNewGame(nlohmann::json& teamsData, 
                     const std::string& leaguePath = "database/league.json", 
                     const std::string& cupPath = "database/cup.json");

    // Simuliert einen Spieltag in ALLEN geladenen Ligen
    void advanceMatchday();

    // Tagesbasierte Simulation & Datumsfortschritt
    void simulateCurrentDay();
    void advanceDateByOneDay();

    // Speichern und Laden der gesamten Spielwelt
    void saveWorld(const std::string& savePath, nlohmann::json& teamsData) const;
    void loadWorld(const std::string& savePath, nlohmann::json& teamsData,
                   const std::string& leaguePath = "database/league.json");

    // Getter für den Zugriff aus dem Menü
    const std::map<std::string, League>& getLeagues() const { return leagues; }
    std::map<std::string, League>& getLeagues() { return leagues; }
    const Cup& getCup() const { return cup; }
    std::string getCurrentDate() const { return currentDate; }
    
    // Hilfsmethode, um eine spezifische Liga zu holen
    League* getLeague(const std::string& name);

private:
    std::map<std::string, League> leagues;
    Cup cup;

    std::string currentDate = "2026-08-01";

    // Interne Hilfsmethoden
    std::map<std::string, League> loadAllLeagues(nlohmann::json& teamsData, const std::string& leaguePath);
};