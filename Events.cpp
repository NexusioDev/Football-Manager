#include <iostream>
#include "Events.hpp"

void GoalEvent(int min, const std::string& team) {
    if (min > 90) std::cout << 90 << "+" << min - 90 << "'" << " - " << "Goal for " << team << std::endl;
    else std::cout << min << "'" << " - " << "Goal for " << team << std::endl;
}

void YellowCardEvent(int min, const std::string& team) {
    if (min > 90) std::cout << 90 << "+" << min - 90 << "'" <<  " - " << "Yellow card for " << team << std::endl;
    else std::cout << min << "'" <<  " - " << "Yellow card for " << team << std::endl;
}

void RedCardEvent(int min, const std::string& team) {
    if (min > 90) std::cout << 90 << "+" << min - 90 << "'" << " - " << "Red card for " << team << std::endl;
    else std::cout << min << "'" << " - " << "Red card for " << team << std::endl;
}