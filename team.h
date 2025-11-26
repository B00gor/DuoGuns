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

    float currentAttackMultiplier = 1.0;
    float currentDefenseMultiplier = 1.0;
    float currentFatigueMultiplier = 1.0;

public:
    Gaster* getPlayer(int index);
    std::vector<int> getShooterIndices();
    std::vector<int> getDefenderIndices();
    int getDefenderForShooter(int shooterIndex);

    Team(const std::string& owner);
    Team(Team&& other) = default;
    Team& operator=(Team&& other) = default;

    void addPlayer(std::unique_ptr<Gaster> player);
    std::vector<std::unique_ptr<Gaster>>& players();
    std::string& owner();
    size_t size();
    bool empty();
    void resetStats();

    void applyTacticEffects(float attackMultiplier, float defenseMultiplier, float fatigueMultiplier);
    void resetTacticEffects();

    void setupDefenseSystem();
    bool isTeamAlive();
    void resetTeam(int endurance);
};

#endif // TEAM_H
