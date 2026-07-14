#include "Cup.hpp"
#include "Match.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

Cup::Cup(std::vector<Team> teamsIn) : teams(std::move(teamsIn)) {
    for (const auto& t : teams) {
        table[t.name] = CupStanding{t.name};
    }
    generateFixtures();
}

void Cup::generateFixtures() {
    fixtures.clear();
    std::shuffle(teams.begin(), teams.end(),rng);
    for (int i = 0; i < teams.size(); i += 2) {
        fixtures.push_back(CupFixture{teams[i].name, teams[i + 1].name});
        fixtures.back().round = teams.size();
    }
    nextFixtureIndex = 0;
    for (int i = 0; i < fixtures.size(); ++i) {
        std::cout << "\n" <<fixtures[i].home << " vs. " << fixtures[i].away;
        if (i + 1 >= fixtures.size()) std::cout << "\n";
    }
}

void Cup::updateStanding(const CupFixture& f, Team home, Team away) {
    /*CupStanding& home = table[f.home];
    CupStanding& away = table[f.away];*/

    if (f.homeGoals > f.awayGoals) {
        winners.push_back(home);
    }
    else if (f.homeGoals < f.awayGoals) {
        winners.push_back(away);
    }

    if (nextFixtureIndex >= fixtures.size()) {
        teams = winners;
        winners.clear();
        printFixtures();
        historyFixtures.insert(historyFixtures.end(), fixtures.begin(), fixtures.end());
        if (teams.size() >= 2) generateFixtures();
    }
}

void Cup::simulateNextFixture() {
    if (nextFixtureIndex >= fixtures.size()) return;

    CupFixture& f = fixtures[nextFixtureIndex];

    auto findTeam = [this](const std::string& name) {
        return *std::find_if(teams.begin(), teams.end(),
            [&](const Team& t) { return t.name == name; });
    };

    Match match(findTeam(f.home), findTeam(f.away));
    match.simulate(true);

    f.homeGoals = match.getHomeGoals();
    f.awayGoals = match.getAwayGoals();
    f.played = true;

    nextFixtureIndex++;
    updateStanding(f, findTeam(f.home), findTeam(f.away));
}

void Cup::simulateAll() {
    while (!isFinished()) {
        simulateNextFixture();
    }
}

bool Cup::isFinished() const {
    return teams.size() == 1;
}

/*void Cup::printTable() const {
    std::vector<CupStanding> sorted;
    for (const auto& [name, s] : table) sorted.push_back(s);

    std::sort(sorted.begin(), sorted.end(), [](const CupStanding& a, const CupStanding& b) {
        if (a.points() != b.points()) return a.points() > b.points();
        if (a.gd() != b.gd()) return a.gd() > b.gd();
        return a.gf > b.gf;
    });

    for (size_t i = 0; i < sorted.size(); ++i) {
        sorted[i].pos = static_cast<int>(i) + 1;
    }

    std::cout << "\n===== Tabelle =====\n";
    std::cout << std::left << std::setw(4) << "Pos" << std::setw(20) << "Team"
               << std::right << std::setw(4) << "Sp"
               << std::setw(4) << "S" << std::setw(4) << "U" << std::setw(4) << "N"
               << std::setw(7) << "Tore" << std::setw(6) << "Pkt" << "\n";

    for (const auto& s : sorted) {
        std::string goals = std::to_string(s.gf) + ":" + std::to_string(s.ga);

        std::cout << std::left << std::setw(4) << s.pos <<std::setw(20) << s.name
                   << std::right << std::setw(4) << s.played
                   << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                   << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\n";
    }
}*/

void Cup::printFixtures() const {
    std::cout << "\n===== Spielplan =====\n";
    for (const auto& f : fixtures) {
        std::cout << f.home << " vs " << f.away;
        if (f.played) std::cout << "  ->  " << f.homeGoals << ":" << f.awayGoals;
        std::cout << "\n";
    }
}

void Cup::printCupFixtures() const {
    std::cout << "\n===== Turnier Verlauf =====\n";
    int oldRound = 0;

    for (const auto& f : historyFixtures) {
        if (f.round != oldRound) {
            std::cout << "\n===== " << roundName(f.round) << " =====\n";
            std::cout << f.home << " vs " << f.away;
            if (f.played) std::cout << "  ->  " << f.homeGoals << ":" << f.awayGoals;
            std::cout << "\n";
            //if (f.played) std::cout << " -> " << f.homeGoals << ":" << f.awayGoals;
        }
        else if (f.round == oldRound) {
            //if (f.played) std::cout << "; " << f.homeGoals << ":" << f.awayGoals;
            std::cout << f.home << " vs " << f.away;
            if (f.played) std::cout << "  ->  " << f.homeGoals << ":" << f.awayGoals;
            std::cout << "\n";
        }
        oldRound = f.round;
    }
    std::cout << "\n" << teams[0].name << " has won the cup!" << "\n";
}

std::string Cup::roundName(int teams) const
{
    switch (teams)
    {
        case 2:  return "Finale";
        case 4:  return "Halbfinale";
        case 8:  return "Viertelfinale";
        case 16: return "Achtelfinale";
        default: return "Runde der " + std::to_string(teams);
    }
}