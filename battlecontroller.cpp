#include "battlecontroller.h"
#include <thread>
#include <chrono>

BattleController::BattleController(std::unique_ptr<Team> playerTeam,
                                   std::unique_ptr<Team> botTeam)
    : playerTeam(std::move(playerTeam)), botTeam(std::move(botTeam)), isPlayerTurn(true) {

    if(!Config::Game::enableCircus) {
        gameEngine.defenseSystem().setupTeamDefense(*this->playerTeam);
        gameEngine.defenseSystem().setupTeamDefense(*this->botTeam);
    }
    setupTactics();
}

void BattleController::performPlayerAttack() {
    if (isGameFinished() || waitingForTactic || !isPlayerTurn) {
        if (!isPlayerTurn) {
            chatManager.addSystemMessage(text("Система: Сейчас ход бота!") | color(Config::UI::systemColor) | bold);
        }
        return;
    }

    if (selectedAttacker == -1 || selectedTarget == -1) {
        chatManager.addSystemMessage(text("Система: Выбери стрелка и цель для атаки!") | color(Config::UI::systemColor) | bold);
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

    isPlayerTurn = false;

    if (!isGameFinished() && !waitingForTactic && !isPlayerTurn) {
        std::thread([this] {
            std::this_thread::sleep_for(std::chrono::milliseconds(Config::Game::attackDelayMs));
            performBotAttack();
        }).detach();
    }
}

void BattleController::performBotAttack() {
    if (isGameFinished() || waitingForTactic || isPlayerTurn) return;

    auto shooterIndices = botTeam->getShooterIndices();
    std::vector<int> aliveShooters;
    for (int index : shooterIndices) {
        if (botTeam->getPlayer(index)->alive()) {
            aliveShooters.push_back(index);
        }
    }

    if (aliveShooters.empty()) {
        chatManager.addDiedGasterMessage(text("У меня не осталось стрелков!") | color(Config::UI::botColor));
        return;
    }

    int botAttackerIdx = aliveShooters[rand() % aliveShooters.size()];
    Gaster* attacker = botTeam->getPlayer(botAttackerIdx);

    int targetIdx = gameEngine.findBestTarget(*playerTeam, *attacker);
    if (targetIdx == -1) {
        chatManager.addDiedGasterMessage(text("Нет доступных целей!") | color(Config::UI::botColor));
        return;
    }

    Gaster* target = playerTeam->getPlayer(targetIdx);

    if (target->role() == "Стрелок") {
        int defenderIndex = gameEngine.defenseSystem().getDefenderForShooter(*playerTeam, targetIdx);
        if (defenderIndex != -1 && playerTeam->getPlayer(defenderIndex)->alive()) {
            target = playerTeam->getPlayer(defenderIndex);
            chatManager.addDiedGasterMessage(text("Атакую защитника, чтобы добраться до стрелка!") | color(Config::UI::botColor));
        }
    }

    bool targetKilled = gameEngine.performAttack(*attacker, *target, false, chatManager);

    if (targetKilled) {
        handleTargetKilled(target);
    }

    if (!isGameFinished()) {
        isPlayerTurn = true;
    }
}

void BattleController::handleRoundEnd(bool playerWon) {
    if (playerWon) {
        playerWins++;
        auto message = text("Система: ВАША КОМАНДА ПОБЕДИЛА В РАУНДЕ!") | color(Config::UI::systemColor) | bold;
        chatManager.addSystemMessage(message);
        isPlayerTurn = false;
    } else {
        botWins++;
        auto message = text("Система: КОМАНДА БОТА ПОБЕДИЛА В РАУНДЕ!") | color(Config::UI::systemColor) | bold;
        chatManager.addSystemMessage(message);
        isPlayerTurn = true;
    }

    auto scoreMessage = text("Система: Счет: " + std::to_string(playerWins) + " - " + std::to_string(botWins)) | color(Config::UI::systemColor) | bold;
    chatManager.addSystemMessage(scoreMessage);

    bool playerWonGame = playerWins >= Config::Game::winCondition;
    bool botWonGame = botWins >= Config::Game::winCondition;

    if (playerWonGame || botWonGame) {
        handleGameEnd();
    }
    else if (isTiebreakerScenario() && Config::Game::enableShootout) {
        if (playerWins == botWins && playerWins == Config::Game::winCondition - 1) {
            handleTiebreaker();
        } else {
            resetForNewRound();
        }
    }
    else {
        resetForNewRound();
    }
}

void BattleController::confirmTactic() {
    if (!waitingForTactic) return;

    std::vector<std::string> tactics = {"Агрессивная", "Осторожная", "Обычный"};

    botTactic = tactics[Config::randomInRange(1,3) % tactics.size()];

    applyTacticEffects();
    waitingForTactic = false;

    auto message = vbox({
                       text("ТАКТИКИ ПОДТВЕРЖДЕНЫ") | bold | color(Color::Green) | center,
                       separator(),
                       hbox({
                           text("Ваша тактика: ") | bold,
                           text(playerTactic) |
                               (playerTactic == "Агрессивная" ? color(Color::Red) :
                                    playerTactic == "Осторожная" ? color(Color::Blue) : color(Color::Green)) | bold
                       }),
                       hbox({
                           text("Тактика бота: ") | bold,
                           text(botTactic) | color(Config::UI::botColor) | bold
                       }),
                       separator(),
                       text("Раунд начинается!") | center | bold | color(Config::UI::systemColor)
                   }) | border;

    chatManager.addSystemMessage(message);

    if (!isPlayerTurn && !isGameFinished()) {
        std::thread([this] {
            std::this_thread::sleep_for(std::chrono::milliseconds(Config::Game::attackDelayMs));
            performBotAttack();
        }).detach();
    }
}

void BattleController::startNewRound() {
    playerTeam->resetTeam(Config::GasterGeneration::endurance);
    botTeam->resetTeam(Config::GasterGeneration::endurance);

    waitingForTactic = true;
    playerTactic = "Обычный";
    botTactic = "Обычный";

    auto roundMessage = text("СИСТЕМА: НАЧИНАЕТСЯ РАУНД " +
                             std::to_string(playerWins + botWins + 1) + "!") |
                        color(Color::Yellow) | bold;
    chatManager.addSystemMessage(roundMessage);

    setupTactics();
}

Team& BattleController::getPlayerTeam() { return *playerTeam; }
Team& BattleController::getBotTeam() { return *botTeam; }
int BattleController::getSelectedAttacker() { return selectedAttacker; }
int BattleController::getSelectedTarget() { return selectedTarget; }
int BattleController::getPlayerWins() { return playerWins; }
int BattleController::getBotWins() { return botWins; }
ChatManager& BattleController::getChatManager() { return chatManager; }
GameEngine& BattleController::getGameEngine() { return gameEngine; }

bool BattleController::isGameFinished() { return playerWins >= Config::Game::winCondition || botWins >= Config::Game::winCondition; }
bool BattleController::isWaitingForTactic() { return waitingForTactic; }
std::string BattleController::getPlayerTactic() { return playerTactic; }
std::string BattleController::getPendingPlayerTactic() { return playerTactic; }
std::string BattleController::getBotTactic() { return botTactic; }

void BattleController::setupTactics() {
    int aggAttack = std::round((Config::Tactic::aggressiveAttackMultiplier - 1.0f) * 100);
    int aggFatigue = std::round((Config::Tactic::aggressiveFatigueMultiplier - 1.0f) * 100);
    int cauDefense = std::round((Config::Tactic::cautiousDefenseMultiplier - 1.0f) * 100);
    int cauAttack = std::round((1.0f - Config::Tactic::cautiousAttackMultiplier) * 100);
    waitingForTactic = true;

    auto message = vbox({
        text("ВЫБЕРИТЕ ТАКТИКУ ДЛЯ ЭТОГО РАУНДА") | bold | color(Color::Yellow) | center,
        separator(),
        text("Доступные тактики:") | bold,
        hbox({ text("Агрессивная: ") | color(Color::Red), text("+" + std::to_string(aggAttack) + "% атака, +" + std::to_string(aggFatigue) + "% усталость") | dim }),
        hbox({ text("Осторожная: ") | color(Color::Blue), text("+" + std::to_string(cauDefense) + "% защита, -" + std::to_string(cauAttack) + "% атака") | dim }),
        hbox({ text("Обычный: ") | color(Color::Green), text("Без изменений") | dim }),
        separator(),
        text("Используйте кнопки ниже для выбора тактики") | dim | center
    }) | border;

    chatManager.addSystemMessage(message);
}

void BattleController::applyTacticEffects() {
    if (playerTactic == "Агрессивная") {
        playerTeam->applyTacticEffects(
            Config::Tactic::aggressiveAttackMultiplier,
            Config::Tactic::aggressiveDefenseMultiplier,
            Config::Tactic::aggressiveFatigueMultiplier
            );
    }
    else if (playerTactic == "Осторожная") {
        playerTeam->applyTacticEffects(
            Config::Tactic::cautiousAttackMultiplier,
            Config::Tactic::cautiousDefenseMultiplier,
            Config::Tactic::cautiousFatigueMultiplier
            );
    }
    else {
        playerTeam->applyTacticEffects(
            Config::Tactic::normalAttackMultiplier,
            Config::Tactic::normalDefenseMultiplier,
            Config::Tactic::normalFatigueMultiplier
            );
    }

    if (botTactic == "Агрессивная") {
        botTeam->applyTacticEffects(
            Config::Tactic::aggressiveAttackMultiplier,
            Config::Tactic::aggressiveDefenseMultiplier,
            Config::Tactic::aggressiveFatigueMultiplier
            );
    }
    else if (botTactic == "Осторожная") {
        botTeam->applyTacticEffects(
            Config::Tactic::cautiousAttackMultiplier,
            Config::Tactic::cautiousDefenseMultiplier,
            Config::Tactic::cautiousFatigueMultiplier
            );
    }
    else {
        botTeam->applyTacticEffects(
            Config::Tactic::normalAttackMultiplier,
            Config::Tactic::normalDefenseMultiplier,
            Config::Tactic::normalFatigueMultiplier
            );
    }
}

void BattleController::handleAttackerSelection(int index) {
    if (isGameFinished()) {
        chatManager.addSystemMessage(text("Система: Игра завершена! Начните новую игру.") | color(Config::UI::systemColor) | bold);
        return;
    }

    if (waitingForTactic) {
        chatManager.addSystemMessage(text("Система: Сначала выберите тактику для этого раунда!") | color(Config::UI::systemColor) | bold);
        return;
    }

    Gaster* player = playerTeam->getPlayer(index);
    if (!player || player->role() != "Стрелок" || !player->alive()) {
        chatManager.addSystemMessage(text("Система: Только живые стрелки могут атаковать!") | color(Config::UI::systemColor) | bold);
        return;
    }

    selectedAttacker = (selectedAttacker == index) ? -1 : index;
    if (selectedAttacker != -1) selectedTarget = -1;
}

void BattleController::handleTargetSelection(int index) {
    if (isGameFinished()) {
        chatManager.addSystemMessage(text("Система: Игра завершена!") | color(Config::UI::systemColor) | bold);
        chatManager.addSystemMessage(text("Начните новую игру.") | color(Config::UI::systemColor) | bold);
        return;
    }

    if (waitingForTactic) {
        chatManager.addSystemMessage(text("Система: Сначала выберите тактику для этого раунда!") | color(Config::UI::systemColor) | bold);
        return;
    }

    if (selectedAttacker == -1) {
        chatManager.addSystemMessage(text("Система: Сначала выбери своего стрелка для атаки!") | color(Config::UI::systemColor) | bold);
        return;
    }

    Gaster* target = botTeam->getPlayer(index);
    if (!target || !target->alive()) {
        chatManager.addSystemMessage(text("Система: Эта цель уже выбыла из боя!") | color(Config::UI::systemColor) | bold);
        return;
    }

    if (target->role() == "Стрелок") {
        int defenderIndex = gameEngine.defenseSystem().getDefenderForShooter(*botTeam, index);
        if (defenderIndex != -1 && botTeam->getPlayer(defenderIndex)->alive()) {
            chatManager.addSystemMessage(text("Система: Этот стрелок защищен!") | color(Config::UI::systemColor) | bold);
            chatManager.addSystemMessage(text("Атакуйте защитника!") | color(Config::UI::systemColor) | bold);
            selectedTarget = defenderIndex;
            return;
        }
    }

    selectedTarget = (selectedTarget == index) ? -1 : index;
}

void BattleController::sendMessage(std::string& message) {
    if (waitingForTactic) {
        chatManager.addSystemMessage(text("Система: Сначала выберите тактику для этого раунда!") | color(Config::UI::systemColor) | bold);
        return;
    }

    if (!message.empty()) {
        auto messageElement = text(message) | color(Config::UI::playerColor);
        chatManager.addPlayerMessage(messageElement);

        std::thread([this, message] {
            std::this_thread::sleep_for(std::chrono::milliseconds(Config::Game::messageDelayMs));

            std::vector<Element> responses = {
                text("Интересно...") | color(Config::UI::botColor),
                text("Моя команда готова!") | color(Config::UI::botColor),
                text("Посмотрим, кто кого!") | color(Config::UI::botColor),
                text("Ты не справишься!") | color(Config::UI::botColor),
                text("Мои стрелки не промахнутся!") | color(Config::UI::botColor),
                text("Твои защитники долго не продержатся!") | color(Config::UI::botColor),
                text("Отличный выстрел! Но я ответлю!") | color(Config::UI::botColor),
                text("Ты хорошо стреляешь, но я лучше!") | color(Config::UI::botColor),
                text("Эта битва будет легендарной!") | color(Config::UI::botColor)
            };

            int randomResponse = rand() % responses.size();
            chatManager.addBotMessage(responses[randomResponse]);
        }).detach();
    }
}

void BattleController::handleTargetKilled(Gaster* target) {
    if (target->role() == "Защитник") {
        chatManager.addSystemMessage(text("Система: Стрелки, которых он защищал, теперь уязвимы!") | color(Config::UI::systemColor) | bold);

        if (target->selectedBy() == "player") {
            playerTeam->setupDefenseSystem();
        } else {
            botTeam->setupDefenseSystem();
        }
    }
    if (isGameFinished()) return;

    if (!playerTeam->isTeamAlive()) {
        handleRoundEnd(false);
    } else if (!botTeam->isTeamAlive()) {
        handleRoundEnd(true);
    }
}

void BattleController::handleGameEnd() {
    if (playerWins >= Config::Game::winCondition) {
        chatManager.addSystemMessage(text("Система: ВЫ ВЫИГРАЛИ МАТЧ СО СЧЕТОМ " +
                                          std::to_string(playerWins) + " - " + std::to_string(botWins) + "!") |
                                     color(Config::UI::systemColor) | bold);
        chatManager.addSystemMessage(text("Система: ПОЗДРАВЛЯЕМ С ПОБЕДОЙ!") | color(Config::UI::systemColor) | bold);
    } else {
        chatManager.addSystemMessage(text("Система: БОТ ВЫИГРАЛ МАТЧ СО СЧЕТОМ " +
                                          std::to_string(playerWins) + " - " + std::to_string(botWins) + "!") |
                                     color(Config::UI::systemColor) | bold);
        chatManager.addSystemMessage(text("Система: ПОПРОБУЙТЕ ЕЩЕ РАЗ!") | color(Config::UI::systemColor) | bold);
    }
}

void BattleController::selectPlayerTactic(const std::string& tactic) {
    if (!waitingForTactic) return;
    playerTactic = tactic;
    applyTacticEffects();
}

void BattleController::handleTiebreaker() {
    if (!playerTeam || !botTeam) {
        handleGameEnd();
        return;
    }

    playerTeam->resetTeam(Config::GasterGeneration::endurance);
    botTeam->resetTeam(Config::GasterGeneration::endurance);
    playerTeam->resetStats();
    botTeam->resetStats();

    playerTeam->setupDefenseSystem();
    botTeam->setupDefenseSystem();

    auto message1 = text("СИСТЕМА: РЕШАЮЩАЯ БИТВА!") | color(Color::Yellow) | bold;
    auto message2 = text("Счет " + std::to_string(playerWins) + ":" + std::to_string(botWins) + " - этот раунд определит победителя!") | color(Config::UI::systemColor) | bold;
    auto message3 = text("Особые правила: участвуют все стрелки,") | color(Config::UI::systemColor) | bold;
    auto message4 = text("выносливость не учитывается!") | color(Color::Green) | bold;

    chatManager.addSystemMessage(message1);
    chatManager.addSystemMessage(message2);
    chatManager.addSystemMessage(message3);
    chatManager.addSystemMessage(message4);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    performTiebreakerRound();
}

void BattleController::performTiebreakerRound() {
    if (!playerTeam || !botTeam) {
        handleGameEnd();
        return;
    }

    std::vector<Element> tiebreakerLines;
    tiebreakerLines.push_back(text("РЕШАЮЩАЯ БИТВА") | bold | color(Color::Yellow) | center);
    tiebreakerLines.push_back(separator());

    std::vector<Gaster*> playerShooters;
    std::vector<Gaster*> botShooters;

    for (int i = 0; i < playerTeam->size(); i++) {
        Gaster* shooter = playerTeam->getPlayer(i);
        if (shooter && shooter->role() == "Стрелок") {
            playerShooters.push_back(shooter);
        }
    }

    for (int i = 0; i < botTeam->size(); i++) {
        Gaster* shooter = botTeam->getPlayer(i);
        if (shooter && shooter->role() == "Стрелок") {
            botShooters.push_back(shooter);
        }
    }

    tiebreakerLines.push_back(hbox({
        text("Участвуют стрелки: ") | bold,
        text("Игрок - ") | color(Config::UI::playerColor),
        text(std::to_string(playerShooters.size())),
        text(" | Бот - ") | color(Config::UI::botColor),
        text(std::to_string(botShooters.size()))
    }));
    tiebreakerLines.push_back(separator());

    int playerHits = 0;
    int playerTotalDamage = 0;
    int botHits = 0;
    int botTotalDamage = 0;

    for (Gaster* shooter : playerShooters) {
        bool hit = (rand() % 100) < shooter->getAccuracy();
        int damage = hit ? shooter->getAttack() : 0;

        if (hit) {
            playerHits++;
            playerTotalDamage += damage;
            tiebreakerLines.push_back(hbox({
                text(shooter->name()) | color(Config::UI::playerColor),
                text(" попадает и наносит ") | color(Color::Green),
                text(std::to_string(damage)) | color(Color::Red) | bold,
                text(" урона!")
            }));
        } else {
            tiebreakerLines.push_back(hbox({
                text(shooter->name()) | color(Config::UI::playerColor),
                text(" промахивается") | color(Color::Red)
            }));
        }
    }

    for (Gaster* shooter : botShooters) {
        bool hit = (rand() % 100) < shooter->getAccuracy();
        int damage = hit ? shooter->getAttack() : 0;

        if (hit) {
            botHits++;
            botTotalDamage += damage;
            tiebreakerLines.push_back(hbox({
                text(shooter->name()) | color(Config::UI::botColor),
                text(" попадает и наносит ") | color(Color::Green),
                text(std::to_string(damage)) | color(Color::Red) | bold,
                text(" урона!")
            }));
        } else {
            tiebreakerLines.push_back(hbox({
                text(shooter->name()) | color(Config::UI::botColor),
                text(" промахивается") | color(Color::Red)
            }));
        }
    }

    tiebreakerLines.push_back(separator());

    tiebreakerLines.push_back(text("ИТОГИ РЕШАЮЩЕЙ БИТВЫ:") | bold | color(Color::Yellow) | center);
    tiebreakerLines.push_back(hbox({
        text("Ваши стрелки: ") | color(Config::UI::playerColor),
        text(std::to_string(playerHits) + " попаданий, ") | bold,
        text(std::to_string(playerTotalDamage) + " урона") | color(Color::Red)
    }));
    tiebreakerLines.push_back(hbox({
        text("Стрелки бота: ") | color(Config::UI::botColor),
        text(std::to_string(botHits) + " попаданий, ") | bold,
        text(std::to_string(botTotalDamage) + " урона") | color(Color::Red)
    }));

    tiebreakerLines.push_back(separator());

    bool playerWonTiebreaker = false;

    if (playerTotalDamage > botTotalDamage) {
        playerWonTiebreaker = true;
        tiebreakerLines.push_back(text("ВАША КОМАНДА ПОБЕДИЛА В РЕШАЮЩЕЙ БИТВЕ!") | color(Config::UI::playerColor) | bold);
    } else if (botTotalDamage > playerTotalDamage) {
        playerWonTiebreaker = false;
        tiebreakerLines.push_back(text("КОМАНДА БОТА ПОБЕДИЛА В РЕШАЮЩЕЙ БИТВЕ!") | color(Config::UI::botColor) | bold);
    } else {
        if (playerHits > botHits) {
            playerWonTiebreaker = true;
            tiebreakerLines.push_back(text("ВАША КОМАНДА ПОБЕДИЛА ПО КОЛИЧЕСТВУ ПОПАДАНИЙ!") | color(Config::UI::playerColor) | bold);
        } else if (botHits > playerHits) {
            playerWonTiebreaker = false;
            tiebreakerLines.push_back(text("КОМАНДА БОТА ПОБЕДИЛА ПО КОЛИЧЕСТВУ ПОПАДАНИЙ!") | color(Config::UI::botColor) | bold);
        } else {
            playerWonTiebreaker = (Config::randomInRange(1,2)% 2 == 0);
            if (playerWonTiebreaker) {
                tiebreakerLines.push_back(text("НИЧЬЯ! ПОБЕДА ДОСТАЛАСЬ ВАМ ПО ЖРЕБИЮ!") | color(Config::UI::playerColor) | bold);
            } else {
                tiebreakerLines.push_back(text("НИЧЬЯ! ПОБЕДА ДОСТАЛАСЬ БОТУ ПО ЖРЕБИЮ!") | color(Config::UI::botColor) | bold);
            }
        }
    }

    auto tiebreakerElement = window(
        text("") | hcenter | bold | color(Color::Yellow),
        vbox(tiebreakerLines)
        );
    chatManager.addSystemMessage(tiebreakerElement);

    if (playerWonTiebreaker) {
        playerWins++;
        auto winMessage = text("Система: ВЫ ВЫИГРАЛИ РЕШАЮЩУЮ БИТВУ И МАТЧ!") | color(Config::UI::playerColor) | bold;
        chatManager.addSystemMessage(winMessage);
    } else {
        botWins++;
        auto winMessage = text("Система: БОТ ВЫИГРАЛ РЕШАЮЩУЮ БИТВУ И МАТЧ!") | color(Config::UI::botColor) | bold;
        chatManager.addSystemMessage(winMessage);
    }

    auto finalScoreMessage = text("Система: Финальный счет: " + std::to_string(playerWins) + " - " + std::to_string(botWins)) | color(Config::UI::systemColor) | bold;
    chatManager.addSystemMessage(finalScoreMessage);

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    handleGameEnd();
}

bool BattleController::isTiebreakerScenario() {
    return (playerWins == botWins &&
            playerWins >= Config::Game::winCondition - 1);
}

void BattleController::resetForNewRound() { startNewRound(); }
