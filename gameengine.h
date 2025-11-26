#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "gaster.h"
#include "team.h"
#include "defensesystem.h"
#include "chatmanager.h"

class GameEngine {
private:
    DefenseSystem defenseSystemInstance;

public:
    GameEngine() = default;
    bool performAttack(Gaster& attacker, Gaster& target, bool isPlayerAttack, ChatManager& chatManager);
    int findBestTarget(Team& enemyTeam, Gaster& attacker);
    void addPlayerMessage(const Element& content);
    void addBotMessage(const Element& content);
    DefenseSystem& defenseSystem();
};

#endif // GAMEENGINE_H
