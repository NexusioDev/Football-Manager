#pragma once

#include <string>

void GoalEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);
void YellowCardEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);
void RedCardEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);