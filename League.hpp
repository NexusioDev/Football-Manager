#pragma once

#include <string>
#include <vector>
#include <map>
#include "Team.hpp"

struct Fixture {
    std::string home;
    std::string away;
    int homeGoals = 0;
    int awayGoals = 0;
    bool played = false;
    std::string date;
};

struct Standing {
    std::string name;
    int played = 0, won = 0, drawn = 0, lost = 0;
    int gf = 0, ga = 0;
    int pos = 0;

    int gd() const { return gf - ga; }
    int points() const { return won * 3 + drawn; }
};

class League {
public:
    League() = default;

    explicit League(std::string nameIn, std::vector<Team> teamsIn,
                   int amountRelegated, int amountRelPlayoff = 0,
                   int amountCl = 0, int amountEl = 0, int amountCfl = 0, std::string above = "", std::string below = "");

    void generateFixtures();
    void simulateFixturesForDate(const std::string& currentDate);
    void simulateNextFixture();
    void simulateNextMatchday();
    void simulateAll();

    bool isFinished() const;
    std::vector<Standing> getSortedTable() const;
    std::vector<Team> getRelegatedTeams() const;
    std::vector<Team> getPromotedTeams(int amountPromoted) const;
    std::string getLeagueAbove() const { return leagueAbove; }
    std::string getLeagueBelow() const { return leagueBelow; }
    void resetForNewSeason(const std::vector<Team>& newTeams);
    void printTable() const;
    void printFixtures() const;
    void printTableFixtures() const;
    Team getRelegationPlayoffTeam() const;
    Team getThirdPlaceTeam() const;

    // Getter & Setter für GameEngine (Speichern/Laden & Status)
    std::string getName() const { return name; }
    const std::vector<Team>& getTeams() const { return teams; }
    void setTeams(const std::vector<Team>& newTeams) { teams = newTeams; }

    size_t getNextFixtureIndex() const { return nextFixtureIndex; }
    void setNextFixtureIndex(size_t idx) { nextFixtureIndex = idx; }

    int getAmountRelegationTeams() const { return amountRelegationTeams; }
    int getAmountRelegationPlayoffTeams() const { return amountRelegationPlayoffTeams; }
    int getAmountChampionsLeagueTeams() const { return amountChampionsLeagueTeams; }
    int getAmountEuropaLeagueTeams() const { return amountEuropaLeagueTeams; }
    int getAmountConferenceLeagueTeams() const { return amountConferenceLeagueTeams; }

    const std::map<std::string, Standing>& getTable() const { return table; }
    void setTable(const std::map<std::string, Standing>& newTable) { table = newTable; }

    int getCurrentMatchday() const {
        if (teams.empty()) return 1;
        return static_cast<int>(nextFixtureIndex / (teams.size() / 2)) + 1;
    }
    Team findTeam(const std::string& teamName) const;

private:
    std::string name;
    std::string leagueAbove;
    std::string leagueBelow;
    std::vector<Team> teams;
    std::vector<Fixture> fixtures;
    std::map<std::string, Standing> table;
    size_t nextFixtureIndex = 0;
    int amountRelegationTeams = 0;
    int amountRelegationPlayoffTeams = 0;
    int amountChampionsLeagueTeams = 0;
    int amountEuropaLeagueTeams = 0;
    int amountConferenceLeagueTeams = 0;

    void updateStanding(const Fixture& f);
};