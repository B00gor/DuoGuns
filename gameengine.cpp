#include "gameengine.h"

DefenseSystem& GameEngine::defenseSystem() {
    return defenseSystemInstance;
}

bool GameEngine::performAttack(Gaster& attacker, Gaster& target, bool isPlayerAttack, ChatManager& chatManager) {
    if (!attacker.alive() || !target.alive()) {
        return false;
    }
    int staminaLossPercent;
    if (attacker.getAttack() >= target.getAttack()) {
        staminaLossPercent = Config::randomInRange(8, 12);
    } else {
        staminaLossPercent = Config::randomInRange(4, 7);
    }
    int staminaLoss = (attacker.endurance() * staminaLossPercent) / 100;
    attacker.setEndurance(std::max(0, attacker.endurance() - staminaLoss));

    int attackerEnduranceLoss = 100 - attacker.endurance();
    int attackerPenaltyStages = attackerEnduranceLoss / 18;
    if (attackerPenaltyStages > 0) {
        int attackPenalty = attackerPenaltyStages * 12;
        int newAttack = attacker.getAttack() * (100 - attackPenalty) / 100;
        attacker.setAttack(std::max(1, newAttack));
    }

    int targetEnduranceLoss = 100 - target.endurance();
    int targetPenaltyStages = targetEnduranceLoss / 18;
    if (targetPenaltyStages > 0) {
        int characteristicPenalty = targetPenaltyStages * 12;

        if (target.getType() == "defender") {
            int newDefense = target.getDefense() * (100 - characteristicPenalty) / 100;
            target.setDefense(std::max(1, newDefense));
        } else {
            int newAttack = target.getAttack() * (100 - characteristicPenalty) / 100;
            target.setAttack(std::max(1, newAttack));
        }
    }

    bool hitSuccess = false;
    int damage = 0;

    if (attacker.getType() == "shooter") {
        if (target.getType() == "defender") {
            if (attacker.getAttack() > target.getDefense()) {
                hitSuccess = (rand() % 100) < attacker.getAccuracy();
            } else {
                hitSuccess = (rand() % 100) >= target.getReaction();
            }
        } else {
            hitSuccess = (rand() % 100) < attacker.getAccuracy();
        }

        if (hitSuccess) {
            damage = attacker.getAttack();
            if (target.getType() == "defender") {
                damage = std::max(1, damage - target.getDefense() / 10);
            }
        }
    }

    if (!hitSuccess) {
        chatManager.addAttackMessage(attacker.name(), target.name(), 0, false, isPlayerAttack);
        return false;
    }

    target.setEndurance(std::max(0, target.endurance() - damage));
    chatManager.addAttackMessage(attacker.name(), target.name(), damage, true, isPlayerAttack);

    if (target.endurance() <= 0) {
        target.setAlive(false);
        chatManager.addSystemMessage("Система: " + target.name() + " выбыл из игры!");
        return true;
    }

    return false;
}

int GameEngine::findBestTarget(Team& enemyTeam, Gaster& attacker) {
    int bestTarget = -1;
    int bestScore = -1;

    for (int i = 0; i < enemyTeam.size(); i++) {
        Gaster* target = enemyTeam.getPlayer(i);
        if (!target || !target->alive()) continue;

        int score = 0;

        if (target->role() == "Стрелок") {
            int defenderIndex = defenseSystemInstance.getDefenderForShooter(enemyTeam, i);
            if (defenderIndex != -1) {
                Gaster* defender = enemyTeam.getPlayer(defenderIndex);
                if (defender && defender->alive()) {
                    continue;
                }
            }
            score = 1000;
        }
        else if (target->role() == "Защитник") {
            score = 800;
        }
        else {
            score = 300;
        }

        score += (100 - target->endurance());

        if (target->role() == "Защитник") {
            score += target->protectedShootersCount() * 50;
        }

        if (score > bestScore) {
            bestScore = score;
            bestTarget = i;
        }
    }

    if (bestTarget == -1) {
        for (int i = 0; i < enemyTeam.size(); i++) {
            Gaster* target = enemyTeam.getPlayer(i);
            if (target && target->alive()) {
                bestTarget = i;
                break;
            }
        }
    }

    return bestTarget;
}
