#include <iostream>
#include "Events.hpp"

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