#pragma once

#include <random>
#include "Team.hpp"

class Match {
private:
    Team homeTeam; Team awayTeam;

    int homeGoals = 0; int awayGoals = 0;
    int penaltyHomeGoals = 0; int penaltyAwayGoals = 0;
    int homePlayers = 11; int awayPlayers = 11;
    int homeYellowCards = 0; int awayYellowCards = 0;
    int extraTime = 0;
    bool inExtraTime = false;

    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> chance10{1, 10};
    std::uniform_int_distribution<int> coinFlip{1, 2};
    std::uniform_int_distribution<int> teamFactor{-15, 15};
    std::uniform_int_distribution<int> penaltyShoot{1, 5};

public:
    Match(Team home, Team away);
    void simulate(bool needWinner = false);
    void simulateEvent(int m, int emin = 0);
    void processEvent(Team &home, Team &opponent, int &goals, int &homePlayers, int opponentPlayers, int &yellowCards, int minute, int extraMinute = 0);

    void printResult();

    int getHomeGoals() const { return homeGoals; }
    int getAwayGoals() const { return awayGoals; }
};