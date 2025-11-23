#include "battlecontroller.h"
#include <thread>
#include <chrono>

BattleController::BattleController(std::unique_ptr<Team> playerTeam,
                                   std::unique_ptr<Team> botTeam)
    : playerTeam(std::move(playerTeam)),
    botTeam(std::move(botTeam)) {

    gameEngine.defenseSystem().setupTeamDefense(*this->playerTeam);
    gameEngine.defenseSystem().setupTeamDefense(*this->botTeam);
}

Team& BattleController::getPlayerTeam() {
    return *playerTeam;
}

Team& BattleController::getBotTeam() {
    return *botTeam;
}

int BattleController::getSelectedAttacker() {
    return selectedAttacker;
}

int BattleController::getSelectedTarget() {
    return selectedTarget;
}

int BattleController::getPlayerWins() {
    return playerWins;
}

int BattleController::getBotWins() {
    return botWins;
}

ChatManager& BattleController::getChatManager() {
    return chatManager;
}

GameEngine& BattleController::getGameEngine() {
    return gameEngine;
}

bool BattleController::isGameFinished() {
    return playerWins >= Config::Game::winCondition || botWins >= Config::Game::winCondition;
}

void BattleController::handleAttackerSelection(int index) {
    if (isGameFinished()) {
        chatManager.addSystemMessage("Система: Игра завершена! Начните новую игру.");
        return;
    }

    Gaster* player = playerTeam->getPlayer(index);
    if (!player || player->role() != "Стрелок" || !player->alive()) {
        chatManager.addSystemMessage("Система: Только живые стрелки могут атаковать!");
        return;
    }

    selectedAttacker = (selectedAttacker == index) ? -1 : index;
    if (selectedAttacker != -1) selectedTarget = -1;
}

void BattleController::handleTargetSelection(int index) {
    if (isGameFinished()) {
        chatManager.addSystemMessage("Система: Игра завершена! Начните новую игру.");
        return;
    }

    if (selectedAttacker == -1) {
        chatManager.addSystemMessage("Система: Сначала выбери своего стрелка для атаки!");
        return;
    }

    Gaster* target = botTeam->getPlayer(index);
    if (!target || !target->alive()) {
        chatManager.addSystemMessage("Система: Эта цель уже выбыла из боя!");
        return;
    }

    if (target->role() == "Стрелок") {
        int defenderIndex = gameEngine.defenseSystem().getDefenderForShooter(*botTeam, index);
        if (defenderIndex != -1 && botTeam->getPlayer(defenderIndex)->alive()) {
            chatManager.addSystemMessage("Система: Этот стрелок защищен! Сначала атакуйте защитника!");
            selectedTarget = defenderIndex;
            return;
        }
    }

    selectedTarget = (selectedTarget == index) ? -1 : index;
}

void BattleController::performPlayerAttack() {
    if (isGameFinished()) {
        chatManager.addSystemMessage("Система: Игра завершена! Начните новую игру.");
        return;
    }

    if (selectedAttacker == -1 || selectedTarget == -1) {
        chatManager.addSystemMessage("Система: Выбери стрелка и цель для атаки!");
        return;
    }

    Gaster* attacker = playerTeam->getPlayer(selectedAttacker);
    Gaster* target = botTeam->getPlayer(selectedTarget);

    if (!attacker || !target) return;

    bool targetKilled = gameEngine.performAttack(*attacker, *target, true, chatManager);

    if (targetKilled) {
        handleTargetKilled(target);
    }

    selectedAttacker = -1;
    selectedTarget = -1;

    if (!isGameFinished()) {
        std::thread([this] {
            std::this_thread::sleep_for(std::chrono::milliseconds(Config::Game::attackDelayMs));
            performBotAttack();
        }).detach();
    }
}

void BattleController::performBotAttack() {
    if (isGameFinished()) return;

    auto shooterIndices = botTeam->getShooterIndices();
    std::vector<int> aliveShooters;
    for (int index : shooterIndices) {
        if (botTeam->getPlayer(index)->alive()) {
            aliveShooters.push_back(index);
        }
    }

    if (aliveShooters.empty()) {
        chatManager.addBotMessage("У меня не осталось стрелков!");
        return;
    }

    int botAttackerIdx = aliveShooters[rand() % aliveShooters.size()];
    Gaster* attacker = botTeam->getPlayer(botAttackerIdx);

    int targetIdx = gameEngine.findBestTarget(*playerTeam, *attacker);
    if (targetIdx == -1) {
        chatManager.addBotMessage("Нет доступных целей!");
        return;
    }

    Gaster* target = playerTeam->getPlayer(targetIdx);

    if (target->role() == "Стрелок") {
        int defenderIndex = gameEngine.defenseSystem().getDefenderForShooter(*playerTeam, targetIdx);
        if (defenderIndex != -1 && playerTeam->getPlayer(defenderIndex)->alive()) {
            target = playerTeam->getPlayer(defenderIndex);
            chatManager.addBotMessage("Атакую защитника, чтобы добраться до стрелка!");
        }
    }

    bool targetKilled = gameEngine.performAttack(*attacker, *target, false, chatManager);

    if (targetKilled) {
        handleTargetKilled(target);
    }
}

void BattleController::sendMessage(std::string& message) {
    if (!message.empty()) {
        chatManager.addPlayerMessage(message);

        std::thread([this, message] {
            std::this_thread::sleep_for(std::chrono::milliseconds(Config::Game::messageDelayMs));
            std::vector<std::string> responses = {
                "Интересно...",
                "Моя команда готова!",
                "Посмотрим, кто кого!",
                "Ты не справишься!",
                "Мои стрелки не промахнутся!",
                "Твои защитники долго не продержатся!",
                "Отличный выстрел! Но я ответлю!",
                "Ты хорошо стреляешь, но я лучше!",
                "Эта битва будет легендарной!"
            };
            int randomResponse = rand() % responses.size();
            chatManager.addBotMessage(responses[randomResponse]);
        }).detach();
    }
}

void BattleController::handleTargetKilled(Gaster* target) {
    if (target->role() == "Защитник") {
        chatManager.addSystemMessage("Система: Стрелки, которых он защищал, теперь уязвимы!");
        if (target->selectedBy() == "player") {
            playerTeam->setupDefenseSystem();
        } else {
            botTeam->setupDefenseSystem();
        }
    }

    if (!playerTeam->isTeamAlive()) {
        botWins++;
        handleRoundEnd(false);
    } else if (!botTeam->isTeamAlive()) {
        playerWins++;
        handleRoundEnd(true);
    }
}

void BattleController::handleRoundEnd(bool playerWon) {
    if (playerWon) {
        chatManager.addSystemMessage("Система: ВАША КОМАНДА ПОБЕДИЛА В РАУНДЕ!");
    } else {
        chatManager.addSystemMessage("Система: КОМАНДА БОТА ПОБЕДИЛА В РАУНДЕ!");
    }

    chatManager.addSystemMessage("Система: Счет: " + std::to_string(playerWins) + " - " + std::to_string(botWins));

    if (playerWins >= Config::Game::winCondition || botWins >= Config::Game::winCondition) {
        handleGameEnd();
    } else {
        playerTeam->resetTeam(Config::GasterGeneration::endurance);
        botTeam->resetTeam(Config::GasterGeneration::endurance);
        playerTeam->setupDefenseSystem();
        botTeam->setupDefenseSystem();
        chatManager.addSystemMessage("Система: Новый раунд начинается!");
    }
}

void BattleController::handleGameEnd() {
    if (playerWins >= Config::Game::winCondition) {
        chatManager.addSystemMessage("Система: ВЫ ВЫИГРАЛИ МАТЧ СО СЧЕТОМ " +
                                     std::to_string(playerWins) + " - " + std::to_string(botWins) + "!");
        chatManager.addSystemMessage("Система: ПОЗДРАВЛЯЕМ С ПОБЕДОЙ!");
    } else {
        chatManager.addSystemMessage("Система: БОТ ВЫИГРАЛ МАТЧ СО СЧЕТОМ " +
                                     std::to_string(playerWins) + " - " + std::to_string(botWins) + "!");
        chatManager.addSystemMessage("Система: ПОПРОБУЙТЕ ЕЩЕ РАЗ!");
    }
}
