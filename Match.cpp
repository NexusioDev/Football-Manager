#include "Match.hpp"
#include "Events.hpp"
#include <iostream>
#include <random>

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
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> chance10(1, 10);
    std::uniform_int_distribution<int> coinFlip(1, 2);
    std::uniform_int_distribution<int> teamFactor(-15, 15);

    std::cout << "\n===== Match Ticker =====\n";

    for (int m = 1; m < 91; m++) {
        int event = chance10(rng); // 10% Chance auf irgendein Event
        if (event == 1) {
            int teamWithEvent = coinFlip(rng); //welches team hat ein event
            if (teamWithEvent == 1) {
                int eventType = chance10(rng); // Was für ein event? 10%rot, 20% gelb, 70% tor
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
                    if (homeTeam.attack * homePlayers / 11 > awayTeam.defense * awayPlayers / 11 + teamFactor(rng)) {
                        homeGoals++;
                        GoalEvent(m, homeTeam.name);
                    }
                }
            }
            else if (teamWithEvent == 2) {
                int eventType = chance10(rng); // Was für ein event? 10%rot, 20% gelb, 70% tor
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
                    if (awayTeam.attack * awayPlayers / 11 > homeTeam.defense * homePlayers / 11 + teamFactor(rng)) {
                        awayGoals++;
                        GoalEvent(m, awayTeam.name);
                    }
                }
            }
        }
    }
}
