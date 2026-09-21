#include "GameEngine.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

std::map<std::string, League> GameEngine::loadAllLeagues(nlohmann::json& teamsData, const std::string& leaguePath) {
    std::ifstream file(leaguePath);
    if (!file.is_open()) {
        throw std::runtime_error("league.json nicht gefunden unter: " + leaguePath);
    }
    nlohmann::json leagueData;
    file >> leagueData;

    std::map<std::string, League> loadedLeagues;

    for (const auto& [leagueName, lJson] : leagueData.items()) {
        std::vector<Team> teams;
        for (const auto& teamNameJson : lJson["teams"]) {
            std::string tName = teamNameJson.get<std::string>();
            if (teamsData.contains(tName)) {
                teams.push_back(loadTeam(teamsData, tName));
            } else {
                throw std::runtime_error("Team \"" + tName + "\" in Liga \"" + leagueName + "\" fehlt in teams.json!");
            }
        }

        int rel = lJson.value("amountRelegationTeams", 2);
        int relPo = lJson.value("amountRelegationPlayoffTeams", 0);
        int cl = lJson.value("amountChampionsLeagueTeams", 0);
        int el = lJson.value("amountEuropaLeagueTeams", 0);
        int cfl = lJson.value("amountConferenceLeagueTeams", 0);

        League league(leagueName, teams, rel, relPo, cl, el, cfl);
        league.generateFixtures();

        loadedLeagues[leagueName] = league;
    }

    return loadedLeagues;
}

void GameEngine::initNewGame(nlohmann::json& teamsData, const std::string& leaguePath, const std::string& cupPath) {
    leagues = loadAllLeagues(teamsData, leaguePath);

    // Cup mit allen verfügbaren Teams initialisieren
    std::vector<Team> allTeams;
    for (const auto& item : teamsData.items()) {
        if (item.key().rfind("_", 0) != 0) {
            allTeams.push_back(loadTeam(teamsData, item.key()));
        }
    }
    cup = Cup(allTeams);
}

void GameEngine::advanceMatchday() {
    std::cout << "\n========================================\n";
    std::cout << " Aktuelles Datum: " << currentDate << "\n";
    std::cout << "========================================\n";
    std::cout << "Simuliere Spieltag in allen Ligen...\n";

    for (auto& [name, league] : leagues) {
        if (!league.isFinished()) {
            league.simulateNextMatchday();
            std::cout << "-> " << name << ": Spieltag " << league.getCurrentMatchday() << " simuliert.\n";
        } else {
            std::cout << "-> " << name << ": Saison beendet.\n";
        }
    }

    advanceDateByOneDay();
}

League* GameEngine::getLeague(const std::string& name) {
    auto it = leagues.find(name);
    if (it != leagues.end()) {
        return &(it->second);
    }
    return nullptr;
}

void GameEngine::saveWorld(const std::string& savePath, nlohmann::json& teamsData) const {
    nlohmann::json data;

    data["version"] = 2; // World-Savegame Version
    data["timestamp"] = std::time(nullptr);
    data["currentDate"] = currentDate;

    // 1. Teams mit aktuellen Werten speichern
    nlohmann::json teamsJson;
    for (const auto& item : teamsData.items()) {
        if (item.key().rfind("_", 0) != 0) {
            Team t = loadTeam(teamsData, item.key());
            teamsJson[t.name] = {{"attack", t.attack}, {"defense", t.defense}};
        }
    }
    data["teams"] = teamsJson;

    // 2. Alle Ligen & Tabellen speichern
    nlohmann::json leaguesJson;
    for (const auto& [leagueName, league] : leagues) {
        nlohmann::json lJson;
        lJson["nextFixtureIndex"] = league.getNextFixtureIndex();
        lJson["amountRelegationTeams"] = league.getAmountRelegationTeams();
        lJson["amountRelegationPlayoffTeams"] = league.getAmountRelegationPlayoffTeams();
        lJson["amountChampionsLeagueTeams"] = league.getAmountChampionsLeagueTeams();
        lJson["amountEuropaLeagueTeams"] = league.getAmountEuropaLeagueTeams();
        lJson["amountConferenceLeagueTeams"] = league.getAmountConferenceLeagueTeams();

        // Tabelle speichern
        nlohmann::json tableJson;
        for (const auto& [tName, standing] : league.getTable()) {
            tableJson[tName] = {
                {"position", standing.pos},
                {"played", standing.played},
                {"won", standing.won},
                {"drawn", standing.drawn},
                {"lost", standing.lost},
                {"gf", standing.gf},
                {"ga", standing.ga}
            };
        }
        lJson["table"] = tableJson;
        leaguesJson[leagueName] = lJson;
    }
    data["leagues"] = leaguesJson;

    // 3. Pokal-Status speichern
    nlohmann::json cupJson;
    cupJson["isFinished"] = cup.isFinished();
    data["cup"] = cupJson;

    // Datei schreiben
    std::ofstream out(savePath);
    if (!out.is_open()) {
        throw std::runtime_error("Fehler beim Öffnen der Speicherdatei: " + savePath);
    }
    out << std::setw(4) << data << std::endl;
    out.close();
}

void GameEngine::loadWorld(const std::string& savePath, nlohmann::json& teamsData, const std::string& leaguePath) {
    std::ifstream in(savePath);
    if (!in.is_open()) {
        throw std::runtime_error("Savegame-Datei nicht gefunden: " + savePath);
    }
    nlohmann::json data;
    in >> data;
    in.close();

    currentDate = data.value("currentDate", "2026-08-01");

    // 1. Basis-Ligenstruktur laden
    leagues = loadAllLeagues(teamsData, leaguePath);

    // 2. Gespeicherte Spielstände über die Ligen legen
    if (data.contains("leagues")) {
        for (auto& [leagueName, lSave] : data["leagues"].items()) {
            if (leagues.find(leagueName) != leagues.end()) {
                leagues[leagueName].setNextFixtureIndex(lSave.value("nextFixtureIndex", 0));

                if (lSave.contains("table")) {
                    std::map<std::string, Standing> restoredTable;
                    for (auto& [tName, sJson] : lSave["table"].items()) {
                        Standing s;
                        s.name   = tName;
                        s.pos    = sJson.value("position", 0);
                        s.played = sJson.value("played", 0);
                        s.won    = sJson.value("won", 0);
                        s.drawn  = sJson.value("drawn", 0);
                        s.lost   = sJson.value("lost", 0);
                        s.gf     = sJson.value("gf", 0);
                        s.ga     = sJson.value("ga", 0);
                        restoredTable[tName] = s;
                    }
                    leagues[leagueName].setTable(restoredTable);
                }
            }
        }
    }
}

void GameEngine::simulateCurrentDay() {
    // Korrektur: Da leagues ein std::map<string, League> ist,
    // muss über Key/Value-Paare iteriert werden:
    for (auto& [name, league] : leagues) {
        league.simulateFixturesForDate(currentDate);
    }

    advanceDateByOneDay();
}

void GameEngine::advanceDateByOneDay() {
    std::tm timeInfo = {};
    std::stringstream ss(currentDate);

    ss >> std::get_time(&timeInfo, "%Y-%m-%d");

    if (ss.fail())
    {
        std::cerr << "Fehler: Datum konnte nicht geladen werden: " << currentDate << std::endl;
        return;
    }

    timeInfo.tm_mday += 1;

    if (std::mktime(&timeInfo) == -1) {
        std::cerr << "Fehler bei der Datumsberechnung für: " << currentDate << std::endl;
        return;
    }

    // Zurück in den String formatieren (garantiert immer YYYY-MM-DD mit führenden Nullen)
    std::stringstream newDate;
    newDate << std::put_time(&timeInfo, "%Y-%m-%d");
    currentDate = newDate.str();
}

void GameEngine::printCurrentDate() {
    // Wandelt "2026-08-01" um in "01.08.2026" nur für die Anzeige
    std::string y = currentDate.substr(0, 4);
    std::string m = currentDate.substr(5, 2);
    std::string d = currentDate.substr(8, 2);

    std::cout << "\n" << "Aktuelles Datum: " << d << "." << m << "." << y << "\n" << std::endl;
}