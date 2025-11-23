#ifndef TEAM_H
#define TEAM_H

#include <vector>
#include <memory>
#include <string>
#include "gaster.h"

class Team {
private:
    std::vector<std::unique_ptr<Gaster>> teamGasters;
    std::string teamOwner;

public:
    Team(const std::string& owner);
    Team(Team&& other) = default;
    Team& operator=(Team&& other) = default;

    void addPlayer(std::unique_ptr<Gaster> player);
    std::vector<std::unique_ptr<Gaster>>& players();
    std::string& owner();
    size_t size();
    bool empty();

    std::vector<int> getShooterIndices();
    std::vector<int> getDefenderIndices();
    void setupDefenseSystem();
    int getDefenderForShooter(int shooterIndex);
    bool isTeamAlive();
    void resetTeam(int endurance);
    Gaster* getPlayer(int index);
};

#endif // TEAM_H
