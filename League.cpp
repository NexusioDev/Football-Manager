#include "League.hpp"
#include "Match.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

// FEHLERBEHEBUNG 1: amountRelegated NICHT mehr minus 1 rechnen!
League::League(std::string nameIn, std::vector<Team> teamsIn, int amountRelegated, int amountRelPlayoff, int amountCl, int amountEl, int amountCfl)
    : name(std::move(nameIn)),
      teams(std::move(teamsIn)),
      amountRelegationTeams(amountRelegated), // Korrigiert!
      amountRelegationPlayoffTeams(amountRelPlayoff),
      amountChampionsLeagueTeams(amountCl),
      amountEuropaLeagueTeams(amountEl),
      amountConferenceLeagueTeams(amountCfl)
{
    for (const auto& t : teams) {
        table[t.name] = Standing{t.name};
    }
    generateFixtures();
}

Team League::findTeam(const std::string& teamName) const {
    auto it = std::find_if(teams.begin(), teams.end(), [&](const Team& t) {
        return t.name == teamName;
    });
    if (it == teams.end()) {
        throw std::runtime_error("Team \"" + teamName + "\" nicht in Liga gefunden!");
    }
    return *it;
}

void League::generateFixtures() {
    fixtures.clear();
    if (teams.size() < 2) return;

    std::vector<Team> tempTeams = teams;
    if (tempTeams.size() % 2 != 0) {
        tempTeams.push_back(Team{"BYE"});
    }

    size_t numTeams = tempTeams.size();
    size_t numRounds = (numTeams - 1) * 2;
    size_t matchesPerRound = numTeams / 2;

    int currentDay = 1;

    for (size_t round = 0; round < numRounds; ++round) {
        for (size_t match = 0; match < matchesPerRound; ++match) {
            size_t homeIdx = (round + match) % (numTeams - 1);
            size_t awayIdx = (numTeams - 1 - match + round) % (numTeams - 1);

            if (match == 0) {
                awayIdx = numTeams - 1;
            }

            if (round >= numTeams - 1) {
                std::swap(homeIdx, awayIdx);
            }

            std::string homeName = tempTeams[homeIdx].name;
            std::string awayName = tempTeams[awayIdx].name;

            if (homeName != "BYE" && awayName != "BYE") {
                Fixture f;
                f.home = homeName;
                f.away = awayName;
                f.date = "2026-08-" + (currentDay < 10 ? "0" + std::to_string(currentDay) : std::to_string(currentDay));
                fixtures.push_back(f);
            }
        }
        currentDay += 7;
    }
    nextFixtureIndex = 0;
}

void League::simulateFixturesForDate(const std::string& currentDate) {
    for (auto& f : fixtures) {
        if (!f.played && f.date == currentDate) {
            Team homeTeam = findTeam(f.home);
            Team awayTeam = findTeam(f.away);

            Match match(homeTeam, awayTeam);
            match.simulate();

            f.homeGoals = match.getHomeGoals();
            f.awayGoals = match.getAwayGoals();
            f.played = true;

            updateStanding(f);
            nextFixtureIndex++; // FEHLERBEHEBUNG: Fortschritt auch bei Tages-Sim mitzählen!
        }
    }
}

void League::updateStanding(const Fixture& f) {
    Standing& home = table[f.home];
    Standing& away = table[f.away];

    home.played++; away.played++;
    home.gf += f.homeGoals; home.ga += f.awayGoals;
    away.gf += f.awayGoals; away.ga += f.homeGoals;

    if (f.homeGoals > f.awayGoals) { home.won++; away.lost++; }
    else if (f.homeGoals < f.awayGoals) { away.won++; home.lost++; }
    else { home.drawn++; away.drawn++; }
}

void League::simulateNextFixture() {
    if (nextFixtureIndex >= fixtures.size()) return;

    Fixture& f = fixtures[nextFixtureIndex];

    Match match(findTeam(f.home), findTeam(f.away));
    match.simulate();

    f.homeGoals = match.getHomeGoals();
    f.awayGoals = match.getAwayGoals();
    f.played = true;

    updateStanding(f);
    nextFixtureIndex++;
}

void League::simulateNextMatchday() {
    if (isFinished() || teams.empty()) return;

    size_t matchesPerMatchday = teams.size() / 2;
    for (size_t i = 0; i < matchesPerMatchday; ++i) {
        if (isFinished()) break;
        simulateNextFixture();
    }
}

void League::simulateAll() {
    while (!isFinished()) {
        simulateNextFixture();
    }
}

bool League::isFinished() const {
    return nextFixtureIndex >= fixtures.size();
}

void League::printTable() const {
    if (teams.empty() || table.empty()) {
        std::cout << "\n[Hinweis] Keine Teams oder Tabellendaten vorhanden.\n";
        return;
    }

    std::vector<Standing> sorted;
    for (const auto& [teamName, s] : table) {
        sorted.push_back(s);
    }

    std::sort(sorted.begin(), sorted.end(), [](const Standing& a, const Standing& b) {
        if (a.points() != b.points()) return a.points() > b.points();
        if (a.gd() != b.gd()) return a.gd() > b.gd();
        return a.gf > b.gf;
    });

    for (size_t i = 0; i < sorted.size(); ++i) {
        sorted[i].pos = static_cast<int>(i) + 1;
    }

    int totalTeams = static_cast<int>(sorted.size());

    // FEHLERBEHEBUNG 2: Exakte Grenzen für Abstieg und Relegation
    int relStart = totalTeams - amountRelegationTeams + 1;
    int relPlayoffStart = relStart - amountRelegationPlayoffTeams;

    std::cout << "\n===== Tabelle (" << name << ") =====\n";
    std::cout << std::left << std::setw(4) << "Pos" << std::setw(28) << "Team"
              << std::right << std::setw(4) << "Sp"
              << std::setw(4) << "S" << std::setw(4) << "U" << std::setw(4) << "N"
              << std::setw(7) << "Tore" << std::setw(6) << "Pkt" << "\n";

    for (const auto& s : sorted) {
        std::string goals = std::to_string(s.gf) + ":" + std::to_string(s.ga);

        size_t len = 0;
        for (unsigned char c : s.name) {
            if ((c & 0xC0) != 0x80) ++len;
        }

        int width = 28 + static_cast<int>(s.name.size() - len);

        if (s.pos >= relStart && amountRelegationTeams > 0) {
            // Direkter Abstieg (Rot)
            std::cout << "\033[41m" << std::left << std::setw(4) << s.pos << std::setw(width) << s.name
                      << std::right << std::setw(4) << s.played
                      << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                      << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\033[0m" << "\n";
        } else if (s.pos >= relPlayoffStart && s.pos < relStart && amountRelegationPlayoffTeams > 0) {
            // Relegationsplatz (Gelb/Orange)
            std::cout << "\033[43m" << std::left << std::setw(4) << s.pos << std::setw(width) << s.name
                      << std::right << std::setw(4) << s.played
                      << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                      << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\033[0m" << "\n";
        } else if (s.pos <= amountChampionsLeagueTeams) {
            // Champions League
            std::cout << "\033[46m" << std::left << std::setw(4) << s.pos << std::setw(width) << s.name
                      << std::right << std::setw(4) << s.played
                      << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                      << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\033[0m" << "\n";
        } else if (s.pos <= amountChampionsLeagueTeams + amountEuropaLeagueTeams) {
            // Europa League
            std::cout << "\033[0;39;48;5;166m" << std::left << std::setw(4) << s.pos << std::setw(width) << s.name
                      << std::right << std::setw(4) << s.played
                      << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                      << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\033[0m" << "\n";
        } else if (s.pos <= amountChampionsLeagueTeams + amountEuropaLeagueTeams + amountConferenceLeagueTeams) {
            // Conference League
            std::cout << "\033[42m" << std::left << std::setw(4) << s.pos << std::setw(width) << s.name
                      << std::right << std::setw(4) << s.played
                      << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                      << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\033[0m" << "\n";
        } else {
            // Normaler Platz
            std::cout << std::left << std::setw(4) << s.pos << std::setw(width) << s.name
                      << std::right << std::setw(4) << s.played
                      << std::setw(4) << s.won << std::setw(4) << s.drawn << std::setw(4) << s.lost
                      << std::setw(8) << goals << std::right << std::setw(5) << s.points() << "\n";
        }
    }
}

void League::printFixtures() const {
    std::cout << "\n===== Spielplan =====\n";
    for (const auto& f : fixtures) {
        std::cout << "[" << f.date << "] " << f.home << " vs. " << f.away;
        if (f.played) {
            std::cout << "  ->  " << f.homeGoals << ":" << f.awayGoals;
        }
        std::cout << "\n";
    }
}

void League::printTableFixtures() const {
    printTable();
    printFixtures();
}