#include "team.h"
#include <iostream>
#include <fstream>
#include <string>

Team::Team(const std::string& owner) : teamOwner(owner) {}

void Team::addPlayer(std::unique_ptr<Gaster> player) {
    teamGasters.push_back(std::move(player));
}

std::vector<std::unique_ptr<Gaster>>& Team::players() { return teamGasters; }
std::string& Team::owner() { return teamOwner; }
size_t Team::size() { return teamGasters.size(); }
bool Team::empty() { return teamGasters.empty(); }

static void log_debug(const std::string& msg) {
    static std::ofstream log("game_debug.log", std::ios::app);
    if (log.is_open()) {
        log << "[DEBUG] " << msg << '\n';
        log.flush();
    }
}

void Team::applyTacticEffects(float attackMultiplier, float defenseMultiplier, float fatigueMultiplier) {
    log_debug("Применение тактик: team=" + teamOwner +
              ", attackMult=" + std::to_string(attackMultiplier) +
              ", defenseMult=" + std::to_string(defenseMultiplier));

    currentAttackMultiplier = attackMultiplier;
    currentDefenseMultiplier = defenseMultiplier;
    currentFatigueMultiplier = fatigueMultiplier;

    for (auto& player : teamGasters) {
        log_debug("До: " + player->name() +
                  " ATK=" + std::to_string(player->getOriginalAttack()) + ">" + std::to_string(player->getAttack()) +
                  " DEF=" + std::to_string(player->getOriginalDefense()) + ">" + std::to_string(player->getDefense()));

        player->applyTacticEffects(attackMultiplier, defenseMultiplier, fatigueMultiplier);

        log_debug("После: " + player->name() +
                  " ATK=" + std::to_string(player->getOriginalAttack()) + ">" + std::to_string(player->getAttack()) +
                  " DEF=" + std::to_string(player->getOriginalDefense()) + ">" + std::to_string(player->getDefense()));
    }
}

void Team::resetTacticEffects() {
    currentAttackMultiplier = 1.0f;
    currentDefenseMultiplier = 1.0f;
    currentFatigueMultiplier = 1.0f;

    for (auto& player : teamGasters) {
        player->resetTacticEffects();
    }
}

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
    resetStats();

    for (auto& player : teamGasters) {
        player->setEndurance(endurance);
        player->setAlive(true);
    }
}

Gaster* Team::getPlayer(int index) {
    return (index >= 0 && index < teamGasters.size()) ? teamGasters[index].get() : nullptr;
}

void Team::resetStats() {
    currentAttackMultiplier = 1.0;
    currentDefenseMultiplier = 1.0;
    currentFatigueMultiplier = 1.0;

    for (auto& player : teamGasters) {
        player->resetTacticEffects();
    }
}
