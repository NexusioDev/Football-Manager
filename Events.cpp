#include <iostream>
#include "Events.hpp"

void GoalEvent(int min, const std::string& team) {
    std::cout << "Goal for " << team << " - " << min << "'" << std::endl;
}

void YellowCardEvent(int min, const std::string& team) {
    std::cout << "Yellow card for " << team << " - " << min << "'" << std::endl;
}

void RedCardEvent(int min, const std::string& team) {
    std::cout << "Red card for " << team << " - " << min << "'" << std::endl;
}