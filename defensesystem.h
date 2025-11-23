#ifndef DEFENSE_SYSTEM_H
#define DEFENSE_SYSTEM_H

#include "team.h"

class DefenseSystem {
public:
    DefenseSystem() = default;
    void setupTeamDefense(Team& team);
    int getDefenderForShooter(Team& team, int shooterIndex);
};

#endif // DEFENSESYSTEM_H
