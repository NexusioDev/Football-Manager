#pragma once

#include <string>
#include <vector>
#include <map>
#include "Team.hpp"

struct Fixture {
    std::string home;
    std::string away;
    int homeGoals = 0;
    int awayGoals = 0;
    bool played = false;
};

struct Standing {
    std::string name;
    int played = 0, won = 0, drawn = 0, lost = 0;
    int gf = 0, ga = 0;
    int pos = 0;

    int gd() const { return gf - ga; }
    int points() const { return won * 3 + drawn; }
};

class League {
public:
    explicit League(std::vector<Team> teams);

    void generateFixtures();       // Hin- und Rückrunde
    void simulateNextFixture();    // ein einzelnes Spiel
    void simulateAll();            // alle verbleibenden Spiele

    bool isFinished() const;
    void printTable() const;
    void printFixtures() const;
    void printTableFixtures() const;

private:
    std::vector<Team> teams;
    std::vector<Fixture> fixtures;
    std::map<std::string, Standing> table;
    size_t nextFixtureIndex = 0;

    void updateStanding(const Fixture& f);
};