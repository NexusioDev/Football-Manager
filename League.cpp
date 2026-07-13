#include "League.hpp"
#include "Match.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

League::League(std::vector<Team> teamsIn) : teams(std::move(teamsIn)) {
    for (const auto& t : teams) {
        table[t.name] = Standing{t.name};
    }
    generateFixtures();
}

void League::generateFixtures() {
    fixtures.clear();
    for (size_t i = 0; i < teams.size(); ++i) {
        for (size_t j = 0; j < teams.size(); ++j) {
            if (i != j) {
                fixtures.push_back(Fixture{teams[i].name, teams[j].name});
            }
        }
    }
    nextFixtureIndex = 0;
}

void League::updateStanding(const Fixture& f) {
    Standing& home = table[f.home];
    Standing& away = table[f.away];

    home.played++; away.played++;
    home.gf += f.homeGoals; home.ga += f.awayGoals;
    away.gf += f.awayGoals; away.ga += f.homeGoals;

    if (f.homeGoals > f.awayGoals) { home.won++; away.lost++; }
    else if (f.homeGoals < f.awayGoals) { away.won++; home.lost++; }
    else { home.drawn++; away.drawn++; }
}

void League::simulateNextFixture() {
    if (nextFixtureIndex >= fixtures.size()) return;

    Fixture& f = fixtures[nextFixtureIndex];

    auto findTeam = [this](const std::string& name) {
        return *std::find_if(teams.begin(), teams.end(),
            [&](const Team& t) { return t.name == name; });
    };

    Match match(findTeam(f.home), findTeam(f.away));
    match.simulate();

    f.homeGoals = match.getHomeGoals();
    f.awayGoals = match.getAwayGoals();
    f.played = true;

    updateStanding(f);
    nextFixtureIndex++;
}

void League::simulateAll() {
    while (!isFinished()) {
        simulateNextFixture();
    }
}

bool League::isFinished() const {
    return nextFixtureIndex >= fixtures.size();
}

void League::printTable() const {
    std::vector<Standing> sorted;
    for (const auto& [name, s] : table) sorted.push_back(s);

    std::sort(sorted.begin(), sorted.end(), [](const Standing& a, const Standing& b) {
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
               << std::setw(6) << "Tore" << std::setw(7) << "Pkt" << "\n";

    for (const auto& s : sorted) {
        std::string goals = std::to_string(s.gf) + ":" + std::to_string(s.ga);

        std::cout << std::left << std::setw(4) << s.pos <<std::setw(20) << s.name
                   << std::right << std::setw(4) << s.played
                   << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                   << std::setw(7) << goals << std::right << std::setw(5) << s.points() << "\n";
    }
}

void League::printFixtures() const {
    std::cout << "\n===== Spielplan =====\n";
    for (const auto& f : fixtures) {
        std::cout << f.home << " vs " << f.away;
        if (f.played) std::cout << "  ->  " << f.homeGoals << ":" << f.awayGoals;
        std::cout << "\n";
    }
}