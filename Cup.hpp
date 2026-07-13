#pragma once

#include <string>
#include <vector>
#include <map>
#include <random>

#include "Team.hpp"

struct CupFixture {
    std::string home;
    std::string away;
    int homeGoals = 0;
    int awayGoals = 0;
    bool played = false;
    int round = 0;
};

struct CupStanding {
    std::string name;
    int startPosition;
};

class Cup {
public:
    explicit Cup(std::vector<Team> teams);

    void generateFixtures();       // Hin- und Rückrunde
    void simulateNextFixture();    // ein einzelnes Spiel
    void simulateAll();            // alle verbleibenden Spiele

    bool isFinished() const;
    //void printTable() const;
    void printFixtures() const;
    void printCupFixtures() const;

    std::string roundName(int teams) const;

private:
    std::mt19937 rng{std::random_device{}()};

    std::vector<Team> teams;
    std::vector<Team> winners;
    std::vector<CupFixture> fixtures;
    std::vector<CupFixture> historyFixtures;
    std::map<std::string, CupStanding> table;
    size_t nextFixtureIndex = 0;

    void updateStanding(const CupFixture& f, Team h, Team a);
};