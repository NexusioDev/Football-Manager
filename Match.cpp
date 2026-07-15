#include "Match.hpp"
#include "Events.hpp"
#include <iostream>
#include <random>

Match::Match(Team home, Team away)
{
    homeTeam = home;
    awayTeam = away;
}

void Match::printResult() const
{
    std::cout << "\n===== Match Result =====\n"
        << homeTeam.name << " " << homeGoals << " : " << awayGoals << " " << awayTeam.name << "\n";
}
//TODO: Verletzungen hinzufügen
void Match::simulate(bool needWinner) {
    std::cout << "\n===== Match Ticker: " << homeTeam.name << " vs " << awayTeam.name << " =====\n";
    extraTime = false;
    for (int m = 1; m < 91; m++) {
        simulateEvent(m);
    }
    for (int e = 1; e <= extraTime; e++) {
        inExtraTime = true;
        simulateEvent(90,e);
    }

    if (homeGoals == awayGoals && needWinner) {
        for (int m = 1; m < 31; m++) {
            inExtraTime = false;
            extraTime = 0;
            simulateEvent(90 + m);
        }
        for (int e = 1; e <= extraTime; e++) {
            inExtraTime = true;
            simulateEvent(120,e);
        }
        if (homeGoals == awayGoals) {
            for (int m = 1; m <= 5; m++) {
                if (penaltyShoot(rng) != 1) {
                    homeGoals++;
                    penaltyHomeGoals++;
                    PenaltyShootoutGoalEvent(homeTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
                else {
                    PenaltyShootoutMissEvent(homeTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
                if (penaltyShoot(rng) != 1) {
                    awayGoals++;
                    penaltyAwayGoals++;
                    PenaltyShootoutGoalEvent(awayTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
                else {
                    PenaltyShootoutMissEvent(awayTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
            }
            while (homeGoals == awayGoals) {
                if (penaltyShoot(rng) != 1) {
                    homeGoals++;
                    penaltyHomeGoals++;
                    PenaltyShootoutGoalEvent(homeTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
                else {
                    PenaltyShootoutMissEvent(homeTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
                if (penaltyShoot(rng) != 1) {
                    awayGoals++;
                    penaltyAwayGoals++;
                    PenaltyShootoutGoalEvent(awayTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
                else {
                    PenaltyShootoutMissEvent(awayTeam.name, penaltyHomeGoals, penaltyAwayGoals);
                }
            }
        }
    }
}

void Match::simulateEvent(int m, int e) {
    if (chance10(rng) == 1) { // 10% Chance auf irgendein Event
        if (coinFlip(rng) == 1) { //welches team hat ein event
            processEvent(homeTeam, awayTeam, homeGoals, homePlayers, awayPlayers, homeYellowCards, m, e);
        } else {
            processEvent(awayTeam, homeTeam, awayGoals, awayPlayers, homePlayers, awayYellowCards, m, e);
        }
    }
}

void Match::processEvent(Team& team, Team& opponent, int& goals, int& players, int opponentPlayers, int& yellowCards, int minute, int eMinute) {
    int eventType = chance10(rng); // Was für ein event? 10%rot, 20% gelb, 70% tor
    if (eventType == 1) { // 1 Rotekarte
        players--;
        extraTime += 2;
        RedCardEvent(minute, team.name, inExtraTime, eMinute);
    }
    else if (eventType < 4) { //2+3 Gelbe Karte
        yellowCards++;
        extraTime++;
        if (yellowCards == 2) {
            players--;
            yellowCards = 0;
        }
        YellowCardEvent(minute, team.name, inExtraTime, eMinute);
    }
    else if (eventType > 3) { // 4-10 Torchance
        double attackStr = team.attack * (players / 11.0);
        double defStr = opponent.defense * (opponentPlayers / 11.0);
        double isHome = (&team == &homeTeam) ? homeAdv : 1.0;

        double xGPerShot = baseXG * pow(attackStr / defStr,exponent) * isHome + flukeXG;
        bool isGoal = (prob(rng) < xGPerShot);

        if (isGoal) {
            goals++;
            GoalEvent(minute, team.name, inExtraTime, eMinute);
        }
    }
}
