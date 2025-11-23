#ifndef TEAMSELECTIONSCREEN_H
#define TEAMSELECTIONSCREEN_H

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "gaster.h"
#include "team.h"
#include "gastergenerator.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

class TeamSelectionScreen {
private:
    GasterGenerator playerGenerator;
    std::vector<std::unique_ptr<Gaster>> allPlayers;
    std::unique_ptr<Team> playerTeam;
    std::unique_ptr<Team> botTeam;

    int playerDefendersCount = 0;
    int playerShootersCount = 0;
    int botDefendersCount = 0;
    int botShootersCount = 0;
    std::string currentTurn = "player";
    bool selectionLocked = false;
    bool selectionCompleted = false;

public:
    TeamSelectionScreen();
    std::pair<std::unique_ptr<Team>, std::unique_ptr<Team>> run();

private:
    void handlePlayerSelection(int index, ScreenInteractive& screen);
    void botMakeMove(ScreenInteractive& screen);
    void updateStats();
    bool isSelectionComplete();
    Element renderPlayerCard(int index, bool isSelected, bool isFocused);
    Element renderUI(std::vector<Component>& playerButtons, std::vector<int>& defenderIndices, std::vector<int>& shooterIndices, Component& backButton);
};

#endif // TEAMSELECTIONSCREEN_H
