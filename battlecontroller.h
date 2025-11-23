#ifndef BATTLECONTROLLER_H
#define BATTLECONTROLLER_H

#include "config.h"
#include "team.h"
#include "gameengine.h"
#include "chatmanager.h"
#include <memory>
#include <functional>

class BattleController {
private:
    std::unique_ptr<Team> playerTeam;
    std::unique_ptr<Team> botTeam;
    GameEngine gameEngine;
    ChatManager chatManager;

    int selectedAttacker = -1;
    int selectedTarget = -1;
    int playerWins = 0;
    int botWins = 0;

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
    bool isGameFinished();

    // Методы управления битвой
    void handleAttackerSelection(int index);
    void handleTargetSelection(int index);
    void performPlayerAttack();
    void sendMessage(std::string& message);

private:
    void performBotAttack();
    void handleTargetKilled(Gaster* target);
    void handleRoundEnd(bool playerWon);
    void handleGameEnd();
};

#endif // BATTLECONTROLLER_H
