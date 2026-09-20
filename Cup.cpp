#include "Cup.hpp"
#include "Match.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

Cup::Cup(std::vector<Team> teamsIn) : teams(std::move(teamsIn)) {
    if (!teams.empty()) {
        generateFixtures();
    }
}

void Cup::generateFixtures() {
    fixtures.clear();
    nextFixtureIndex = 0;

    // Wenn weniger als 2 Teams da sind, kann keine Runde mehr generiert werden
    if (teams.size() < 2) {
        return;
    }

    // Teams für zufällige Auslosung mischen
    std::shuffle(teams.begin(), teams.end(), rng);

    int currentRound = static_cast<int>(teams.size());

    // Paarungen bilden (Immer 2 aufeinanderfolgende Teams gegeneinander)
    for (size_t i = 0; i + 1 < teams.size(); i += 2) {
        CupFixture f;
        f.home = teams[i].name;
        f.away = teams[i + 1].name;
        f.round = currentRound;
        f.played = false;
        fixtures.push_back(f);
    }

    winners.clear();
}

void Cup::simulateNextFixture() {
    // 1. Sichere Abbruchbedingung gegen Leere Vektoren / Falsche Indizes
    if (fixtures.empty() || nextFixtureIndex >= fixtures.size()) {
        return;
    }

    // Nutzen von .at() wirft eine Exception statt eines Debug-Assertion-Crashes
    CupFixture& f = fixtures.at(nextFixtureIndex);

    auto findTeam = [this](const std::string& teamName) {
        auto it = std::find_if(teams.begin(), teams.end(), [&](const Team& t) {
            return t.name == teamName;
        });
        if (it == teams.end()) {
            throw std::runtime_error("Team im Pokal nicht gefunden: " + teamName);
        }
        return *it;
    };

    Team homeTeam = findTeam(f.home);
    Team awayTeam = findTeam(f.away);

    Match match(homeTeam, awayTeam);
    match.simulate();

    f.homeGoals = match.getHomeGoals();
    f.awayGoals = match.getAwayGoals();

    // Pokal-Verlängerung/Elfmeterschießen bei Unentschieden
    if (f.homeGoals == f.awayGoals) {
        std::uniform_int_distribution<int> dist(0, 1);
        if (dist(rng) == 0) {
            f.homeGoals++;
        } else {
            f.awayGoals++;
        }
    }

    f.played = true;

    // Sieger ermitteln und für die nächste Runde speichern
    if (f.homeGoals > f.awayGoals) {
        f.winner = f.home;
        winners.push_back(homeTeam);
    } else {
        f.winner = f.away;
        winners.push_back(awayTeam);
    }

    historyFixtures.push_back(f);
    nextFixtureIndex++;

    // 2. Wenn alle Spiele der aktuellen Runde vorbei sind -> Nächste Runde vorbereiten!
    if (nextFixtureIndex >= fixtures.size()) {
        teams = winners; // Die Sieger rücken in die nächste Runde auf
        generateFixtures(); // Erstellt die nächsten Paarungen
    }
}

void Cup::simulateAll() {
    while (!isFinished()) {
        simulateNextFixture();
    }
}

bool Cup::isFinished() const {
    // Der Pokal ist erst vorbei, wenn nur noch 1 Gewinner übrig ist und keine Spiele mehr anstehen
    return teams.size() <= 1 && (fixtures.empty() || nextFixtureIndex >= fixtures.size());
}

std::string Cup::roundName(int teamCount) const {
    switch (teamCount) {
        case 2: return "Finale";
        case 4: return "Halbfinale";
        case 8: return "Viertelfinale";
        case 16: return "Achtelfinale";
        default: return "Runde der letzten " + std::to_string(teamCount);
    }
}

void Cup::printFixtures() const {
    std::cout << "\n===== Pokal Spiele =====\n";
    for (const auto& f : historyFixtures) {
        std::cout << "[" << roundName(f.round) << "] "
                  << f.home << " " << f.homeGoals << ":" << f.awayGoals << " " << f.away
                  << " (Sieger: " << f.winner << ")\n";
    }
}

void Cup::printCupFixtures() const {
    printFixtures();
}

void Cup::updateStanding(const CupFixture& f, Team w) {
    // Bisher ungenutzt / Platzhalter
}