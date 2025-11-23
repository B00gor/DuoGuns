#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include "battlecontroller.h"
#include "config.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

class BattleView {
private:
    BattleController& controller;
    std::string currentMessage = "";

    Component messageInput;
    Component sendButton;
    Component attackButton;
    Component chatComponent;
    Component leftTeamContainer;
    Component rightTeamContainer;
    Component centerContainer;
    Component mainContainer;

    std::vector<Component> leftButtons;
    std::vector<Component> rightButtons;

public:
    BattleView(BattleController& controller);
    void run();

private:
    std::vector<Component> createTeamButtons(Team& team, bool isPlayerTeam);
    Component createTeamContainer(Team& team, std::vector<Component>& buttons, const std::string& title, Color titleColor, bool reverseOrder);
    Element renderCenterPanel(Component& chatComponent, Component& messageInput, Component& sendButton, Component& attackButton);
    Element renderSelectionInfo();
    Element renderPlayerInfo(int index, Team& team, const std::string& title, Color titleColor);
    Element renderInstructions();
};

#endif // BATTLEVIEW_H
