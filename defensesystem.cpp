#include "defensesystem.h"

void DefenseSystem::setupTeamDefense(Team& team) {
    team.setupDefenseSystem();
}

int DefenseSystem::getDefenderForShooter(Team& team, int shooterIndex) {
    return team.getDefenderForShooter(shooterIndex);
}
