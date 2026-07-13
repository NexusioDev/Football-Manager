#include <iostream>
#include "Events.hpp"

void GoalEvent(int min, const std::string& team) {
    std::cout << min << "'" << " - " << "Goal for " << team << std::endl;
}

void YellowCardEvent(int min, const std::string& team) {
    std::cout << min << "'" <<  " - " << "Yellow card for " << team << std::endl;
}

void RedCardEvent(int min, const std::string& team) {
    std::cout << min << "'" << " - " << "Red card for " << team << std::endl;
}