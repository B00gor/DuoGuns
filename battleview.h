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

    std::vector<Component> leftButtons;
    std::vector<Component> rightButtons;

    int calculateTemporaryStat(int baseStat, const std::string& tactic, bool isAttackStat);

public:
    BattleView(BattleController& controller);
    void run();

private:
    std::vector<Component> createTeamButtons(Team& team, bool isPlayerTeam);
    Component createTeamContainer(Team& team, std::vector<Component>& buttons, const std::string& title, Color titleColor, bool reverseOrder, bool isPlayerTeam);
    Element renderSelectionInfo();
    Element renderPlayerInfo(int index, Team& team, const std::string& title, Color titleColor, bool isPlayerTeam);
    Element renderInstructions();
    Element renderTacticSelection(const Component& aggressiveBtn, const Component& cautiousBtn, const Component& normalBtn, const Component& confirmBtn);
};

#endif // BATTLEVIEW_H
