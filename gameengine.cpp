#include "gameengine.h"

DefenseSystem& GameEngine::defenseSystem() {
    return defenseSystemInstance;
}

bool GameEngine::performAttack(Gaster& attacker, Gaster& target, bool isPlayerAttack, ChatManager& chatManager) {
    if (!attacker.alive() || !target.alive()) {
        return false;
    }

    std::vector<Element> attackLines;
    int originalAttackerAttack = attacker.getAttack();
    int originalAttackerEndurance = attacker.endurance();
    int originalTargetDefense = target.getDefense();
    int originalTargetEndurance = target.endurance();

    bool positionLossEvent = false;
    bool misfireEvent = false;
    int temporaryDefenseReduction = 0;
    int originalTargetDefenseValue = originalTargetDefense;

    attackLines.push_back(hbox({
        text(attacker.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor) | bold,
        text(" атакует "),
        text(target.name()) | color(isPlayerAttack ? Config::UI::botColor : Config::UI::playerColor) | bold
    }));

    if (target.getType() == "defender" && (rand() % 100) < 15) {
        positionLossEvent = true;
        temporaryDefenseReduction = static_cast<int>(originalTargetDefense * 0.2f);
        int newDefense = std::max(1, originalTargetDefense - temporaryDefenseReduction);

        attackLines.push_back(hbox({
            text(target.name()) | color(isPlayerAttack ? Config::UI::botColor : Config::UI::playerColor) | bold,
            text(" потерял позицию!") | color(Color::Red) | bold,
            text(" Защита -") | dim,
            text(std::to_string(temporaryDefenseReduction)) | color(Color::Red) | bold,
            text(" (") | dim,
            text(std::to_string(originalTargetDefense)) | color(Color::Blue) | dim,
            text(" → ") | dim,
            text(std::to_string(newDefense)) | color(Color::Blue) | bold,
            text(")") | dim
        }));

        originalTargetDefense = newDefense;
    }

    bool hitSuccess = false;
    int damage = 0;

    if (attacker.getType() == "shooter") {
        if (target.getType() == "defender") {
            if (attacker.getAttack() > originalTargetDefense) {
                hitSuccess = (rand() % 100) < attacker.getAccuracy();
                attackLines.push_back(hbox({
                    text("Атака ") | dim,
                    text(std::to_string(attacker.getAttack())) | color(Color::Red) | bold,
                    text(" > Защита ") | dim,
                    text(std::to_string(originalTargetDefense)) | color(Color::Blue) | bold,
                    text(": ") | dim,
                    text(std::to_string(attacker.getAccuracy()) + "%") | color(Color::GreenYellow)
                }));
            } else {
                hitSuccess = (rand() % 100) > target.getReaction();
                attackLines.push_back(hbox({
                    text("Атака ") | dim,
                    text(std::to_string(attacker.getAttack())) | color(Color::Red) | bold,
                    text(" ≤ Защита ") | dim,
                    text(std::to_string(originalTargetDefense)) | color(Color::Blue) | bold,
                    text(": ") | dim,
                    text(std::to_string(100 - target.getReaction()) + "%") | color(Color::GreenYellow)
                }));
            }
        } else {
            if (attacker.endurance() >= target.endurance()) {
                hitSuccess = (rand() % 100) < attacker.getAccuracy();
                attackLines.push_back(hbox({
                    text("Выносл. ") | dim,
                    text(std::to_string(attacker.endurance()) + "%") | color(Color::Green) | bold,
                    text(" ≥ ") | dim,
                    text(std::to_string(target.endurance()) + "%") | color(Color::Green) | bold,
                    text(": ") | dim,
                    text(std::to_string(attacker.getAccuracy()) + "%") | color(Color::GreenYellow)
                }));
            } else {
                hitSuccess = (rand() % 100) < 10;
                attackLines.push_back(hbox({
                    text("Выносл. ") | dim,
                    text(std::to_string(attacker.endurance()) + "%") | color(Color::Green) | bold,
                    text(" < ") | dim,
                    text(std::to_string(target.endurance()) + "%") | color(Color::Green) | bold,
                    text(": 10%") | color(Color::GreenYellow)
                }));
            }
        }

        if (hitSuccess && (rand() % 100) < 15) {
            misfireEvent = true;
            damage = attacker.getAttack();
            int misfireDamageReduction = static_cast<int>(damage * 0.2f);
            damage = std::max(1, damage - misfireDamageReduction);

            attackLines.push_back(hbox({
                text(attacker.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor) | bold,
                text(" осечка!") | color(Color::Red) | bold,
                text(" Урон -") | dim,
                text(std::to_string(misfireDamageReduction)) | color(Color::Red) | bold,
                text(" (") | dim,
                text(std::to_string(attacker.getAttack())) | color(Color::Red) | dim,
                text(" → ") | dim,
                text(std::to_string(damage)) | color(Color::Red) | bold,
                text(")") | dim
            }));
        } else if (hitSuccess) {
            damage = attacker.getAttack();
            if (target.getType() == "defender") {
                damage = std::max(1, damage - (originalTargetDefense / 10));
            }
        }
    }

    bool isFightingStrongerOpponent = attacker.getType() == "shooter" &&
                                      attacker.endurance() < target.endurance() &&
                                      target.getType() == "shooter";

    int staminaLossPercent = isFightingStrongerOpponent ?
                                 Config::randomInRange(4, 7) :
                                 Config::randomInRange(8, 12);

    float fatigueMultiplier = attacker.getFatigueMultiplier();
    int staminaLoss = static_cast<int>((attacker.endurance() * staminaLossPercent) / 100.0f * fatigueMultiplier);
    int newAttackerEndurance = std::max(0, attacker.endurance() - staminaLoss);
    attacker.setEndurance(newAttackerEndurance);

    attackLines.push_back(hbox({
        text(attacker.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor) | bold,
        text(" -"),
        text(std::to_string(staminaLoss)) | color(Color::RedLight) | bold,
        text(" вынос.("),
        text(std::to_string(originalAttackerEndurance)) | color(Color::GreenYellow) | bold,
        text(" → "),
        text(std::to_string(newAttackerEndurance)) | color(newAttackerEndurance > 0 ? Color::Green : Color::Red) | bold,
        text(")")
    }));

    int attackerEnduranceLoss = 100 - attacker.endurance();
    int attackerPenaltyStages = attackerEnduranceLoss / 18;

    if (attackerPenaltyStages > 0) {
        int penaltyPercent = attackerPenaltyStages * 12;

        if (attacker.getType() == "shooter") {
            int newAttack = static_cast<int>(originalAttackerAttack * (100 - penaltyPercent) / 100.0f);
            attacker.setAttackWithUpdate(std::max(1, newAttack));

            attackLines.push_back(hbox({
                text(attacker.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor),
                text(" устал: атака "),
                text(std::to_string(originalAttackerAttack)) | color(Color::Green) | bold,
                text(" → "),
                text(std::to_string(newAttack)) | color(Color::Red) | bold,
                text(" (штраф -"),
                text(std::to_string(penaltyPercent) + "%") | color(Color::Red) | bold,
                text(")")
            }));
        }
    }

    if (!hitSuccess) {
        attackLines.push_back(hbox({
            text(attacker.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor) | bold,
            text(" промахнулся по "),
            text(target.name()) | color(Color::GreenYellow) | bold,
            text("!")
        }));
    } else {
        attackLines.push_back(hbox({
            text(attacker.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor) | bold,
            text(" нанес "),
            text(std::to_string(damage)) | color(Color::Red) | bold,
            text(" урона "),
            text(target.name()) | color(Color::GreenYellow) | bold,
            text("!")
        }));

        int newTargetEndurance = std::max(0, target.endurance() - damage);
        target.setEndurance(newTargetEndurance);

        attackLines.push_back(hbox({
            text(target.name()) | color(isPlayerAttack ? Config::UI::botColor : Config::UI::playerColor),
            text(" получил урон: "),
            text(std::to_string(damage)) | color(Color::Red) | bold,
            text(" (выносл. "),
            text(std::to_string(originalTargetEndurance)) | color(Color::GreenYellow) | bold,
            text(" → "),
            text(std::to_string(newTargetEndurance)) | color(newTargetEndurance > 0 ? Color::Green : Color::Red) | bold,
            text(")")
        }));

        if (newTargetEndurance <= 0) {
            target.setAlive(false);
            auto defeatElement = hbox({
                text(target.name()) | color(isPlayerAttack ? Config::UI::botColor : Config::UI::playerColor) | bold,
                text(" выбыл из боя!") | color(Color::Red) | bold
            });
            chatManager.addDiedGasterMessage(defeatElement);
            auto systemElement = hbox({
                                     text("Система: "),
                                     text(target.name()) | color(Config::UI::systemColor) | bold,
                                     text(" выбыл из игры!")
                                 }) | color(Config::UI::systemColor) | bold;
            chatManager.addSystemMessage(systemElement);

            if (positionLossEvent) {
                target.setDefense(originalTargetDefenseValue);
            }

            return true;
        }
    }

    if (positionLossEvent) {
        target.setDefense(originalTargetDefenseValue);
    }
    int targetEnduranceLoss = 100 - target.endurance();
    int targetPenaltyStages = targetEnduranceLoss / 18;

    if (targetPenaltyStages > 0) {
        int penaltyPercent = targetPenaltyStages * 12;
        if (target.getType() == "defender") {
            int newDefense = static_cast<int>(originalTargetDefenseValue * (100 - penaltyPercent) / 100.0f);
            target.setDefenseWithUpdate(std::max(1, newDefense));

            attackLines.push_back(hbox({
                text(target.name()) | color(isPlayerAttack ? Config::UI::playerColor : Config::UI::botColor),
                text(" устал: защита "),
                text(std::to_string(originalTargetDefenseValue)) | color(Color::Blue) | bold,
                text(" → "),
                text(std::to_string(newDefense)) | color(Color::Red) | bold,
                text(" (штраф -"),
                text(std::to_string(penaltyPercent) + "%") | color(Color::Red) | bold,
                text(")")
            }));
        }
    }
    chatManager.addAttackMessage(vbox(attackLines), isPlayerAttack);
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
