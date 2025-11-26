#ifndef BATTLECONTROLLER_H
#define BATTLECONTROLLER_H

#include "config.h"
#include "team.h"
#include "gameengine.h"
#include "chatmanager.h"
#include <memory>
#include <string>

class BattleController {
private:
    std::unique_ptr<Team> playerTeam;
    std::unique_ptr<Team> botTeam;
    GameEngine gameEngine;
    ChatManager chatManager;
    bool isPlayerTurn;

    int selectedAttacker = -1;
    int selectedTarget = -1;
    int playerWins = 0;
    int botWins = 0;

    bool waitingForTactic = true;
    std::string playerTactic = "Обычный";
    std::string botTactic = "Обычный";

    void performBotAttack();
    void handleTargetKilled(Gaster* target);
    void handleRoundEnd(bool playerWon);
    void handleGameEnd();
    void applyTacticEffects();
    void setupTactics();

public:
    BattleController(std::unique_ptr<Team> playerTeam, std::unique_ptr<Team> botTeam);

    Team& getPlayerTeam();
    Team& getBotTeam();
    int getSelectedAttacker();
    int getSelectedTarget();
    int getPlayerWins();
    int getBotWins();
    ChatManager& getChatManager();
    GameEngine& getGameEngine();
    std::string getPlayerTactic();
    std::string getPendingPlayerTactic();
    std::string getBotTactic();

    bool isGameFinished();
    bool isWaitingForTactic();
    bool isPlayerTurnActive() { return isPlayerTurn; }

    void handleAttackerSelection(int index);
    void handleTargetSelection(int index);
    void performPlayerAttack();
    void sendMessage(std::string& message);

    void selectPlayerTactic(const std::string& tactic);
    void confirmTactic();
    void startNewRound();

    void handleTiebreaker();
    void performTiebreakerRound();
    bool isTiebreakerScenario();
    void resetForNewRound();
};

#endif
