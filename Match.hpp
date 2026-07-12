#pragma once

#include "Team.hpp"

class Match {
private:
    Team homeTeam; Team awayTeam;

    int homeGoals = 0; int awayGoals = 0;
    int homePlayers = 11; int awayPlayers = 11;
    int homeYellowCards = 0; int awayYellowCards = 0;

public:
    Match(Team home, Team away);
    void simulate();
    void printResult();
};