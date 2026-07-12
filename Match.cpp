#include "Match.hpp"
#include "Events.hpp"
#include <cstdlib>
#include <iostream>

Match::Match(Team home, Team away)
{
    homeTeam = home;
    awayTeam = away;
}

void Match::printResult()
{
    std::cout << "\n===== Match Result =====\n"
        << homeTeam.name << " " << homeGoals << " : " << awayGoals << " " << awayTeam.name << "\n";
}
//TODO: Nachspielzeiten hinzufügen + Verletzungen
void Match::simulate() {
    for (int m = 1; m < 91; m++) {
        int event = rand() % 10 + 1; // 10% Chance auf irgendein Event
        if (event == 1) {
            int teamInPlay = rand() % 2 + 1; //welches team hat ein event
            if (teamInPlay == 1) {
                int eventType = rand() % 10 + 1; // Was für ein event? 10%rot, 20% gelb, 70% tor
                if (eventType == 1) { // 1 Rotekarte
                    homePlayers--;
                    RedCardEvent(m, homeTeam.name);
                }
                else if (eventType < 4) { //2+3 Gelbe Karte
                    homeYellowCards++;
                    if (homeYellowCards == 2) {
                        homePlayers--;
                        homeYellowCards = 0;
                    }
                    YellowCardEvent(m, homeTeam.name);
                }
                else if (eventType > 3) { // 4-10 Torchance
                    if (homeTeam.attack * homePlayers / 11 > awayTeam.defense * awayPlayers / 11 + (rand() % 31 - 15)) {
                        homeGoals++;
                        GoalEvent(m, homeTeam.name);
                    }
                }
            }
            else if (teamInPlay == 2) {
                int eventType = rand() % 10 + 1; // Was für ein event? 10%rot, 20% gelb, 70% tor
                if (eventType == 1) { // 1 Rotekarte
                    awayPlayers--;
                    RedCardEvent(m, awayTeam.name);
                }
                else if (eventType < 4) { //2+3 Gelbe Karte
                    awayYellowCards++;
                    if (awayYellowCards == 2) {
                        awayPlayers--;
                        awayYellowCards = 0;
                    }
                    YellowCardEvent(m, awayTeam.name);
                }
                else if (eventType > 3) { // 4-10 Torchance
                    if (awayTeam.attack * awayPlayers / 11 > homeTeam.defense * homePlayers / 11 + (rand() % 31 - 15)) {
                        awayGoals++;
                        GoalEvent(m, awayTeam.name);
                    }
                }
            }
        }
    }
}
