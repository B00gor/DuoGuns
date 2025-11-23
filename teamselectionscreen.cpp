#include "teamselectionscreen.h"
#include "config.h"
#include <thread>
#include <chrono>

TeamSelectionScreen::TeamSelectionScreen() {
    allPlayers = playerGenerator.generateAllPlayers();
    playerTeam = std::make_unique<Team>("player");
    botTeam = std::make_unique<Team>("bot");
}

std::pair<std::unique_ptr<Team>, std::unique_ptr<Team>> TeamSelectionScreen::run() {
    auto screen = ScreenInteractive::Fullscreen();

    std::vector<Component> playerButtons;
    for (int i = 0; i < allPlayers.size(); i++) {
        playerButtons.push_back(Button(
            "",
            [this, i, &screen] { handlePlayerSelection(i, screen); },
            [this, i](const ButtonOption& defaultOption) {
                auto opt = defaultOption;
                opt.transform = [this, i](const EntryState& state) {
                    return renderPlayerCard(i, false, state.focused);
                };
                return opt;
            }(ButtonOption::Ascii())
            ));
    }


    std::vector<int> defenderIndices;
    std::vector<int> shooterIndices;
    for (int i = 0; i < allPlayers.size(); i++) {
        if (allPlayers[i]->role() == "Защитник") {
            defenderIndices.push_back(i);
        } else {
            shooterIndices.push_back(i);
        }
    }

    auto defendersContainer = Container::Vertical({});
    auto shootersContainer = Container::Vertical({});

    for (int index : defenderIndices) {
        defendersContainer->Add(playerButtons[index]);
    }
    for (int index : shooterIndices) {
        shootersContainer->Add(playerButtons[index]);
    }

    auto backButton = Button("Назад", [&] {
        selectionCompleted = false;
        screen.Exit();
    });

    auto rightPanelContainer = Container::Vertical({ defendersContainer, shootersContainer });
    auto mainContainer = Container::Horizontal({ Container::Vertical({ backButton }), rightPanelContainer });

    auto renderer = Renderer(mainContainer, [&] {
        return renderUI(playerButtons, defenderIndices, shooterIndices, backButton);
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            selectionCompleted = false;
            screen.Exit();
            return true;
        }
        if (event == Event::Character('q') || event == Event::CtrlC) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(component);

    if (selectionCompleted) {
        for (auto& player : allPlayers) {
            if (player->selectedBy() == "player") {
                playerTeam->addPlayer(std::move(player));
            } else if (player->selectedBy() == "bot") {
                botTeam->addPlayer(std::move(player));
            }
        }
        return {std::move(playerTeam), std::move(botTeam)};
    }

    return {std::make_unique<Team>("player"), std::make_unique<Team>("bot")};
}

void TeamSelectionScreen::handlePlayerSelection(int index, ScreenInteractive& screen) {
    if (selectionLocked || currentTurn != "player") return;

    Gaster* player = allPlayers[index].get();
    if (!player->canBeSelected("player", player->role() == "Защитник" ? playerDefendersCount : playerShootersCount)) {
        return;
    }

    player->setSelectedBy("player");
    updateStats();

    if (isSelectionComplete()) {
        selectionCompleted = true;
        screen.Exit();
        return;
    }

    selectionLocked = true;
    currentTurn = "bot";

    std::thread([this, &screen] {
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::Game::botMoveDelayMs));
        botMakeMove(screen);
    }).detach();
}

void TeamSelectionScreen::botMakeMove(ScreenInteractive& screen) {
    int bestIndex = -1;
    int bestScore = -1;

    for (int i = 0; i < allPlayers.size(); i++) {
        Gaster* player = allPlayers[i].get();
        if (player->selectedBy() != "") continue;

        if (!player->canBeSelected("bot", player->role() == "Защитник" ? botDefendersCount : botShootersCount)) {
            continue;
        }

        int score = player->getPrimaryStat() + player->getSecondaryStat();
        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }

    if (bestIndex != -1) {
        allPlayers[bestIndex]->setSelectedBy("bot");
        updateStats();
    }

    if (isSelectionComplete()) {
        selectionCompleted = true;
        screen.Exit();
        return;
    }

    currentTurn = "player";
    selectionLocked = false;

    screen.PostEvent(Event::Custom);
}

void TeamSelectionScreen::updateStats() {
    playerDefendersCount = 0;
    playerShootersCount = 0;
    botDefendersCount = 0;
    botShootersCount = 0;

    for (auto& player : allPlayers) {
        if (player->selectedBy() == "player") {
            if (player->role() == "Защитник") playerDefendersCount++;
            else playerShootersCount++;
        } else if (player->selectedBy() == "bot") {
            if (player->role() == "Защитник") botDefendersCount++;
            else botShootersCount++;
        }
    }
}

bool TeamSelectionScreen::isSelectionComplete() {
    return (playerDefendersCount == Config::Team::defendersCount &&
            playerShootersCount == Config::Team::shootersCount &&
            botDefendersCount == Config::Team::defendersCount &&
            botShootersCount == Config::Team::shootersCount);
}

Element TeamSelectionScreen::renderPlayerCard(int index, bool isSelected, bool isFocused) {
    Gaster* player = allPlayers[index].get();
    return player->renderCard(isSelected, isFocused);
}

Element TeamSelectionScreen::renderUI(std::vector<Component>& playerButtons,
                                      std::vector<int>& defenderIndices,
                                      std::vector<int>& shooterIndices,
                                      Component& backButton) {
    std::vector<Element> leftElements;
    std::vector<Element> rightElements;

    leftElements.push_back(text("ВАША КОМАНДА") | bold | color(Config::UI::playerColor) | center);
    leftElements.push_back(separator());

    leftElements.push_back(hbox({
                               text("Вы: ") | color(Config::UI::playerColor),
                               text("Стрелки " + std::to_string(playerShootersCount) + "/" + std::to_string(Config::Team::shootersCount)) |
                                   (playerShootersCount == Config::Team::shootersCount ? color(Config::UI::playerColor) : color(Color::Red)),
                               text(" | "),
                               text("Защитники " + std::to_string(playerDefendersCount) + "/" + std::to_string(Config::Team::defendersCount)) |
                                   (playerDefendersCount == Config::Team::defendersCount ? color(Config::UI::playerColor) : color(Color::Red))
                           }) | center);

    leftElements.push_back(hbox({
                               text("Бот: ") | color(Config::UI::botColor),
                               text("Стрелки " + std::to_string(botShootersCount) + "/" + std::to_string(Config::Team::shootersCount)) |
                                   (botShootersCount == Config::Team::shootersCount ? color(Config::UI::botColor) : color(Color::Yellow)),
                               text(" | "),
                               text("Защитники " + std::to_string(botDefendersCount) + "/" + std::to_string(Config::Team::defendersCount)) |
                                   (botDefendersCount == Config::Team::defendersCount ? color(Config::UI::botColor) : color(Color::Yellow))
                           }) | center);

    leftElements.push_back(separator());
    leftElements.push_back(hbox({
                               text("Сейчас ход: ") | bold,
                               (currentTurn == "player" ? text("ВАШ") | bold | color(Config::UI::playerColor) :
                                    text("БОТА") | bold | color(Config::UI::botColor))
                           }) | center);
    leftElements.push_back(separator());

    std::vector<Element> shootersColumn;
    std::vector<Element> defendersColumn;

    shootersColumn.push_back(text("СТРЕЛКИ") | bold | color(Config::UI::shooterColor) | center);
    shootersColumn.push_back(separator());
    for (int i = 0; i < allPlayers.size(); i++) {
        if (allPlayers[i]->selectedBy() == "player" && allPlayers[i]->role() == "Стрелок") {
            shootersColumn.push_back(playerButtons[i]->Render() | flex);
        }
    }

    defendersColumn.push_back(text("ЗАЩИТНИКИ") | bold | color(Config::UI::defenderColor) | center);
    defendersColumn.push_back(separator());
    for (int i = 0; i < allPlayers.size(); i++) {
        if (allPlayers[i]->selectedBy() == "player" && allPlayers[i]->role() == "Защитник") {
            defendersColumn.push_back(playerButtons[i]->Render() | flex);
        }
    }

    if (playerDefendersCount + playerShootersCount > 0) {
        leftElements.push_back(hbox({
                                   vbox(shootersColumn) | flex,
                                   separator(),
                                   vbox(defendersColumn) | flex
                               }) | flex);
    } else {
        leftElements.push_back(vbox({
                                   text("Пока нет выбранных игроков") | dim | center,
                                   text("Выберите игроков справа") | dim | center
                               }) | flex);
    }

    leftElements.push_back(separator());
    leftElements.push_back(backButton->Render() | center);

    rightElements.push_back(text("ВЫБОР КОМАНДЫ") | bold | color(Color::Yellow) | center);
    rightElements.push_back(separator());
    rightElements.push_back(hbox({
                                text("Зеленый - ваш выбор") | color(Config::UI::playerColor),
                                text(" | "),
                                text("Красный - выбор бота") | color(Config::UI::botColor)
                            }) | center);
    rightElements.push_back(text("Нажмите на игрока чтобы добавить в команду") | dim | center);
    rightElements.push_back(separator());

    rightElements.push_back(text("ДОСТУПНЫЕ ЗАЩИТНИКИ") | bold | color(Config::UI::defenderColor) | center);
    std::vector<Element> defenderElements;
    for (int index : defenderIndices) {
        defenderElements.push_back(playerButtons[index]->Render() | flex);
    }
    rightElements.push_back(hbox(defenderElements));
    rightElements.push_back(separator());

    rightElements.push_back(text("ДОСТУПНЫЕ СТРЕЛКИ") | bold | color(Config::UI::shooterColor) | center);
    std::vector<Element> shootersRows;

    for (int i = 0; i < shooterIndices.size(); i += 4) {
        std::vector<Element> rowElements;
        for (int j = i; j < std::min(i + 4, (int)shooterIndices.size()); j++) {
            int index = shooterIndices[j];
            rowElements.push_back(playerButtons[index]->Render() | flex);
        }
        shootersRows.push_back(hbox(rowElements));
    }

    for (auto& row : shootersRows) {
        rightElements.push_back(row);
    }

    return hbox({
               vbox(leftElements) | flex | border,
               separator(),
               vbox(rightElements) | flex | border
           }) | flex;
}
