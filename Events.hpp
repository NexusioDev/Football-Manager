#pragma once

#include <string>

void PenaltyShootoutGoalEvent(const std::string& team, int homePenaltyGoals, int awayPenaltyGoals);
void PenaltyShootoutMissEvent(const std::string& team, int homePenaltyGoals, int awayPenaltyGoals);
void PenaltyGoalEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);
void GoalEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);
void YellowCardEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);
void RedCardEvent(int min, const std::string& team, bool inExtraTime = false, int extraTimeMin = 0);