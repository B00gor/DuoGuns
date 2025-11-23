#include "team.h"

Team::Team(const std::string& owner) : teamOwner(owner) {}

void Team::addPlayer(std::unique_ptr<Gaster> player) {
    teamGasters.push_back(std::move(player));
}

std::vector<std::unique_ptr<Gaster>>& Team::players() { return teamGasters; }
std::string& Team::owner() { return teamOwner; }
size_t Team::size() { return teamGasters.size(); }
bool Team::empty() { return teamGasters.empty(); }

std::vector<int> Team::getShooterIndices() {
    std::vector<int> indices;
    for (int i = 0; i < teamGasters.size(); i++) {
        if (teamGasters[i]->role() == "Стрелок") {
            indices.push_back(i);
        }
    }
    return indices;
}

std::vector<int> Team::getDefenderIndices() {
    std::vector<int> indices;
    for (int i = 0; i < teamGasters.size(); i++) {
        if (teamGasters[i]->role() == "Защитник") {
            indices.push_back(i);
        }
    }
    return indices;
}

void Team::setupDefenseSystem() {
    auto shooterIndices = getShooterIndices();
    auto defenderIndices = getDefenderIndices();

    for (auto& player : teamGasters) {
        player->clearProtectedShooters();
    }

    for (int i = 0; i < defenderIndices.size(); i++) {
        int defenderIdx = defenderIndices[i];
        for (int j = i * Config::Defense::shootersPerDefender;
             j < (i + 1) * Config::Defense::shootersPerDefender && j < shooterIndices.size(); j++) {
            teamGasters[defenderIdx]->addProtectedShooter(shooterIndices[j]);
        }
    }
}

int Team::getDefenderForShooter(int shooterIndex) {
    for (int i = 0; i < teamGasters.size(); i++) {
        if (teamGasters[i]->role() == "Защитник" && teamGasters[i]->alive()) {
            for (int protectedIdx : teamGasters[i]->protectedShooterIndices()) {
                if (protectedIdx == shooterIndex) {
                    return i;
                }
            }
        }
    }
    return -1;
}

bool Team::isTeamAlive() {
    for (auto& player : teamGasters) {
        if (player->alive()) return true;
    }
    return false;
}

void Team::resetTeam(int endurance) {
    for (auto& player : teamGasters) {
        player->setEndurance(endurance);
        player->setAlive(true);
    }
}

Gaster* Team::getPlayer(int index) {
    return (index >= 0 && index < teamGasters.size()) ? teamGasters[index].get() : nullptr;
}
