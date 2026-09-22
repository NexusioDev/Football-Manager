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
        std::string above = lJson.value("above", "");
        std::string below = lJson.value("below", "");

        League league(leagueName, teams, rel, relPo, cl, el, cfl, above, below);
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

    data["version"] = 3;
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
        lJson["above"] = league.getLeagueAbove();
        lJson["below"] = league.getLeagueBelow();

        // WICHTIG: Die aktuellen Teams der Liga speichern!
        nlohmann::json currentTeamsJson = nlohmann::json::array();
        for (const auto& t : league.getTeams()) {
            currentTeamsJson.push_back(t.name);
        }
        lJson["teams"] = currentTeamsJson;

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

    std::ofstream out(savePath);
    if (!out.is_open()) {
        throw std::runtime_error("Fehler beim Öffnen der Speicherdatei: " + savePath);
    }
    out << std::setw(4) << data << std::endl;
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
                League& league = leagues[leagueName];

                league.setNextFixtureIndex(lSave.value("nextFixtureIndex", 0));

                // A) Teams aus Savegame laden (falls vorhanden)
                if (lSave.contains("teams")) {
                    std::vector<Team> restoredTeams;
                    for (const auto& tNameJson : lSave["teams"]) {
                        std::string tName = tNameJson.get<std::string>();
                        if (teamsData.contains(tName)) {
                            restoredTeams.push_back(loadTeam(teamsData, tName));
                        }
                    }
                    league.setTeams(restoredTeams);
                }

                // B) Spielplan für die geladenen Teams neu aufbauen
                league.generateFixtures();
                league.setNextFixtureIndex(lSave.value("nextFixtureIndex", 0));

                // C) Tabelle wiederherstellen
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
                    league.setTable(restoredTable);
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

bool GameEngine::areAllLeaguesFinished() const {
    for (const auto& [name, league] : leagues) {
        if (!league.isFinished()) {
            return false;
        }
    }
    return true;
}

void GameEngine::startNextSeason() {
    if (!areAllLeaguesFinished()) {
        std::cout << "Saison noch nicht beendet!\n";
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << " VERARBEITE AUTOMATISCHEN AUF- UND ABSTIEG\n";
    std::cout << "========================================\n";

    // Map zum Zwischenspeichern aller neuen Kader für die nächste Saison
    // Key: Ligenname, Value: Neue Liste an Teams
    std::map<std::string, std::vector<Team>> newLeagueTeams;

    // 1. Initialisieren der neuen Ligen mit den bisherigen Teams
    for (const auto& [name, league] : leagues) {
        newLeagueTeams[name] = league.getTeams();
    }

    // 2. Automatisch Absteiger nach unten und Aufsteiger nach oben verschieben
    for (const auto& [name, league] : leagues) {
        std::string lowerLeagueName = league.getLeagueBelow();

        // Wenn diese Liga eine untergeordnete Liga hat, findet Abstieg statt
        if (!lowerLeagueName.empty() && leagues.find(lowerLeagueName) != leagues.end()) {

            // a) Absteiger aus der aktuellen (höheren) Liga holen
            std::vector<Team> relegated = league.getRelegatedTeams();

            // b) Aufsteiger aus der unteren Liga holen
            // (Anzahl der Aufsteiger = Anzahl Absteiger der höheren Liga)
            int numToPromote = league.getAmountRelegationTeams();
            std::vector<Team> promoted = leagues[lowerLeagueName].getPromotedTeams(numToPromote);

            std::cout << "\n[Tausch] " << name << " <---> " << lowerLeagueName << ":\n";

            // c) Absteiger aus 'name' entfernen und in 'lowerLeagueName' einfügen
            for (const auto& relTeam : relegated) {
                std::cout << "  ↓ Absteiger: " << relTeam.name << "\n";

                // Aus höherer Liga löschen
                auto& higherVec = newLeagueTeams[name];
                higherVec.erase(std::remove_if(higherVec.begin(), higherVec.end(),
                    [&](const Team& t) { return t.name == relTeam.name; }), higherVec.end());

                // In untere Liga einfügen
                newLeagueTeams[lowerLeagueName].push_back(relTeam);
            }

            // d) Aufsteiger aus 'lowerLeagueName' entfernen und in 'name' einfügen
            for (const auto& promTeam : promoted) {
                std::cout << "  ↑ Aufsteiger: " << promTeam.name << "\n";

                // Aus unterer Liga löschen
                auto& lowerVec = newLeagueTeams[lowerLeagueName];
                lowerVec.erase(std::remove_if(lowerVec.begin(), lowerVec.end(),
                    [&](const Team& t) { return t.name == promTeam.name; }), lowerVec.end());

                // In höhere Liga einfügen
                newLeagueTeams[name].push_back(promTeam);
            }
        }
    }

    // 3. Alle Ligen mit ihren neuen Team-Zusammenstellungen zurücksetzen
    for (auto& [name, league] : leagues) {
        league.resetForNewSeason(newLeagueTeams[name]);
    }

    // 4. Datum auf das neue Jahr setzen (01. August)
    int nextYear = std::stoi(currentDate.substr(0, 4)) + 1;
    currentDate = std::to_string(nextYear) + "-08-01";

    std::cout << "\n========================================\n";
    std::cout << " Neue Saison " << nextYear << " gestartet!\n";
    std::cout << "========================================\n";
}

RelegationResult GameEngine::simulateRelegationPlayoff(const League& higherLeague, const League& lowerLeague) {
    // 1. Richtige Teams aus den Tabellen ermitteln:
    Team higherTeam = higherLeague.getRelegationPlayoffTeam(); // Platz 16 der höheren Liga
    Team lowerTeam = lowerLeague.getThirdPlaceTeam();           // Platz 3 der unteren Liga

    std::cout << "\n========================================\n";
    std::cout << " RELEGATION PLAYOFF: " << higherTeam.name << " vs. " << lowerTeam.name << "\n";
    std::cout << "========================================\n";

    // 2. Hinspiel (beim Dritten der unterklassigen Liga)
    Match leg1(lowerTeam, higherTeam);
    leg1.simulate();

    // 3. Rückspiel (beim Relegationsplatz der höherklassigen Liga)
    Match leg2(higherTeam, lowerTeam);
    leg2.simulate();

    std::cout << "Hinspiel:  " << lowerTeam.name << " " << leg1.getHomeGoals() << ":" << leg1.getAwayGoals() << " " << higherTeam.name << "\n";
    std::cout << "Rückspiel: " << higherTeam.name << " " << leg2.getHomeGoals() << ":" << leg2.getAwayGoals() << " " << lowerTeam.name << "\n";

    int higherTotal = leg1.getAwayGoals() + leg2.getHomeGoals();
    int lowerTotal = leg1.getHomeGoals() + leg2.getAwayGoals();

    RelegationResult result;
    if (lowerTotal > higherTotal) {
        result.winner = lowerTeam;
        result.loser = higherTeam;
    } else if (higherTotal > lowerTotal) {
        result.winner = higherTeam;
        result.loser = lowerTeam;
    } else {
        // Bei Gleichstand: Zufall / Elfmeterschießen
        if (rand() % 2 == 0) {
            std::cout << "(Entscheidung nach Elfmeterschießen)\n";
            result.winner = lowerTeam;
            result.loser = higherTeam;
        } else {
            std::cout << "(Entscheidung nach Elfmeterschießen)\n";
            result.winner = higherTeam;
            result.loser = lowerTeam;
        }
    }
    return result;
}

void GameEngine::startNextSeasonWithPlayoffs() {
    if (!areAllLeaguesFinished()) {
        std::cout << "Saison ist noch nicht beendet!\n";
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << " VERARBEITE AUTOMATISCHEN AUF- UND ABSTIEG\n";
    std::cout << "========================================\n";

    std::map<std::string, std::vector<Team>> newLeagueTeams;
    for (const auto& [name, league] : leagues) {
        newLeagueTeams[name] = league.getTeams();
    }

    for (auto& [name, higherLeague] : leagues) {
        std::string lowerName = higherLeague.getLeagueBelow();

        if (!lowerName.empty() && leagues.find(lowerName) != leagues.end()) {
            League& lowerLeague = leagues[lowerName];

            // Direkte Absteiger (Platz 17 & 18) und direkte Aufsteiger (Platz 1 & 2)
            std::vector<Team> relegated = higherLeague.getRelegatedTeams();
            int numToPromote = higherLeague.getAmountRelegationTeams();
            std::vector<Team> promoted = lowerLeague.getPromotedTeams(numToPromote);

            // Relegation ausführen, wenn die obere Liga 1 Playoff-Team vorsieht
            if (higherLeague.getAmountRelegationPlayoffTeams() > 0) {
                RelegationResult relResult = simulateRelegationPlayoff(higherLeague, lowerLeague);

                // Wenn der Gewinner der Drittplatzierte der UNTEREN Liga ist:
                if (relResult.winner.name != higherLeague.getRelegationPlayoffTeam().name) {
                    std::cout << "[Relegation] " << relResult.winner.name << " steigt AUF! "
                              << relResult.loser.name << " steigt AB!\n";
                    promoted.push_back(relResult.winner);
                    relegated.push_back(relResult.loser);
                } else {
                    std::cout << "[Relegation] " << relResult.winner.name << " verteidigt den Klassenerhalt!\n";
                }
            }

            std::cout << "\n[Tausch] " << name << " <---> " << lowerName << ":\n";

            for (const auto& relTeam : relegated) {
                std::cout << "  ↓ Absteiger: " << relTeam.name << "\n";
                auto& higherVec = newLeagueTeams[name];
                higherVec.erase(std::remove_if(higherVec.begin(), higherVec.end(),
                    [&](const Team& t) { return t.name == relTeam.name; }), higherVec.end());

                newLeagueTeams[lowerName].push_back(relTeam);
            }

            for (const auto& promTeam : promoted) {
                std::cout << "  ↑ Aufsteiger: " << promTeam.name << "\n";
                auto& lowerVec = newLeagueTeams[lowerName];
                lowerVec.erase(std::remove_if(lowerVec.begin(), lowerVec.end(),
                    [&](const Team& t) { return t.name == promTeam.name; }), lowerVec.end());

                newLeagueTeams[name].push_back(promTeam);
            }
        }
    }

    for (auto& [name, league] : leagues) {
        league.resetForNewSeason(newLeagueTeams[name]);
    }

    int nextYear = std::stoi(currentDate.substr(0, 4)) + 1;
    currentDate = std::to_string(nextYear) + "-08-01";

    std::cout << "\n========================================\n";
    std::cout << " Neue Saison " << nextYear << " gestartet!\n";
    std::cout << "========================================\n";
}