#include <iostream>
#include "Events.hpp"

void PenaltyShootoutGoalEvent(const std::string& team, int homePenaltyGoals, int awayPenaltyGoals) {
    std::cout << "[PSO]" << " - " << "Penalty Goal for " << team << " (" << homePenaltyGoals << ":" << awayPenaltyGoals << ")" << std::endl;
}

void PenaltyShootoutMissEvent(const std::string& team, int homePenaltyGoals, int awayPenaltyGoals) {
    std::cout << "[PSO]" << " - " << "Penalty Miss for " << team << " (" << homePenaltyGoals << ":" << awayPenaltyGoals << ")" << std::endl;
}

void PenaltyGoalEvent(int min, const std::string &team, bool inExtraTime, int eMin) {
    if (inExtraTime) std::cout << min << "+" << eMin << "'" << " - " << "Penalty Goal for " << team << std::endl;
    else std::cout << min << "'" << " - " << "Penalty Goal for " << team << std::endl;
}

void GoalEvent(int min, const std::string& team, bool inExtraTime, int eMin) {
    if (inExtraTime) std::cout << min << "+" << eMin << "'" << " - " << "Goal for " << team << std::endl;
    else std::cout << min << "'" << " - " << "Goal for " << team << std::endl;
}

void YellowCardEvent(int min, const std::string& team, bool inExtraTime, int eMin) {
    if (inExtraTime) std::cout << 90 << "+" << eMin << "'" <<  " - " << "Yellow card for " << team << std::endl;
    else std::cout << min << "'" <<  " - " << "Yellow card for " << team << std::endl;
}

void RedCardEvent(int min, const std::string& team, bool inExtraTime, int eMin) {
    if (inExtraTime) std::cout << min << "+" << eMin << "'" << " - " << "Red card for " << team << std::endl;
    else std::cout << min << "'" << " - " << "Red card for " << team << std::endl;
}