#include "application.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <cstdlib>
#include <ctime>

void Application::run() {
    srand(time(0));

    while (!shouldExit) {
        if (currentScreen == 0) {
            showMainMenu();
        }
        else if (currentScreen == 1) {
            showSingleplayer();
        }
        else if (currentScreen == 2) {
            showMultiplayer();
        }
        else if (currentScreen == 3) {
            showSettings();
        }
        else if (currentScreen == -1) {
            shouldExit = true;
        }
    }
}

void Application::showMainMenu() {
    auto screen = ScreenInteractive::Fullscreen();
    MainMenu mainMenu;

    auto menuComponent = mainMenu.create([&](int choice) {
        if (choice == 0) {
            shouldExit = true;
            screen.Exit();
        } else {
            currentScreen = choice;
            screen.Exit();
        }
    });

    auto component = CatchEvent(menuComponent, [&](Event event) {
        if (event == Event::Escape || event == Event::Character('q') || event == Event::CtrlC) {
            shouldExit = true;
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(component);
}

void Application::showSingleplayer() {
    TeamSelectionScreen selectionScreen;
    auto teams = selectionScreen.run();

    if (!teams.first->empty() && !teams.second->empty()) {
        BattleController controller(std::move(teams.first), std::move(teams.second));
        BattleView view(controller);
        view.run();
    }

    currentScreen = 0;
}

void Application::showMultiplayer() {
    auto screen = ScreenInteractive::Fullscreen();
    auto backButton = Button("Назад", [&] { screen.Exit(); });
    auto container = Container::Vertical({ backButton });

    auto renderer = Renderer(container, [&] {
        std::vector<Element> elements;
        elements.push_back(text("МУЛЬТИПЛЕЕР") | bold | color(Config::UI::cyanColor) | center);
        elements.push_back(separator());
        elements.push_back(text("Режим в разработке") | bold | center);
        elements.push_back(separator());
        elements.push_back(backButton->Render() | center);
        return vbox(elements) | border | center;
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape || event == Event::Character('q') || event == Event::CtrlC) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(component);
    currentScreen = 0;
}

void Application::showSettings() {
    auto screen = ScreenInteractive::Fullscreen();

    std::string winConditionStr = std::to_string(Config::Game::winCondition);
    std::string shooterAttackMinStr = std::to_string(Config::GasterGeneration::shooterAttackMin);
    std::string shooterAttackMaxStr = std::to_string(Config::GasterGeneration::shooterAttackMax);
    std::string shooterAccuracyMinStr = std::to_string(Config::GasterGeneration::shooterAccuracyMin);
    std::string shooterAccuracyMaxStr = std::to_string(Config::GasterGeneration::shooterAccuracyMax);
    std::string defenderDefenseMinStr = std::to_string(Config::GasterGeneration::defenderDefenseMin);
    std::string defenderDefenseMaxStr = std::to_string(Config::GasterGeneration::defenderDefenseMax);
    std::string defenderReactionMinStr = std::to_string(Config::GasterGeneration::defenderReactionMin);
    std::string defenderReactionMaxStr = std::to_string(Config::GasterGeneration::defenderReactionMax);
    std::string botDelayStr = std::to_string(Config::Game::botMoveDelayMs);
    std::string attackDelayStr = std::to_string(Config::Game::attackDelayMs);
    std::string defendersCountStr = std::to_string(Config::Team::defendersCount);
    std::string shootersCountStr = std::to_string(Config::Team::shootersCount);
    std::string shootersPerDefenderStr = std::to_string(Config::Defense::shootersPerDefender);

    auto winConditionInput = Input(&winConditionStr, "5");
    auto shooterAttackMinInput = Input(&shooterAttackMinStr, "50");
    auto shooterAttackMaxInput = Input(&shooterAttackMaxStr, "90");
    auto shooterAccuracyMinInput = Input(&shooterAccuracyMinStr, "70");
    auto shooterAccuracyMaxInput = Input(&shooterAccuracyMaxStr, "95");
    auto defenderDefenseMinInput = Input(&defenderDefenseMinStr, "60");
    auto defenderDefenseMaxInput = Input(&defenderDefenseMaxStr, "100");
    auto defenderReactionMinInput = Input(&defenderReactionMinStr, "65");
    auto defenderReactionMaxInput = Input(&defenderReactionMaxStr, "90");
    auto botDelayInput = Input(&botDelayStr, "100");
    auto attackDelayInput = Input(&attackDelayStr, "500");
    auto defendersCountInput = Input(&defendersCountStr, "2");
    auto shootersCountInput = Input(&shootersCountStr, "4");
    auto shootersPerDefenderInput = Input(&shootersPerDefenderStr, "2");

    auto backButton = Button("Назад", [&] {
        screen.Exit();
    });

    auto leftColumn = Container::Vertical({
        winConditionInput,
        shooterAttackMinInput,
        shooterAttackMaxInput,
        shooterAccuracyMinInput,
        shooterAccuracyMaxInput,
        defenderDefenseMinInput,
        defenderDefenseMaxInput,
        defenderReactionMinInput,
        defenderReactionMaxInput
    });

    auto rightColumn = Container::Vertical({
        defendersCountInput,
        shootersCountInput,
        shootersPerDefenderInput,
        botDelayInput,
        attackDelayInput
    });

    auto container = Container::Vertical({
        Container::Horizontal({ leftColumn, rightColumn }),
        backButton
    });

    auto renderer = Renderer(container, [&] {
        auto leftCol = vbox({
            text("Баланс игры") | center | bold,
            separator(),
            hbox({ text("Побед для победы: "), winConditionInput->Render()  }),
            separator(),
            text("Характеристики стрелков") | center | bold,
            separator(),
            hbox({ text("Мин. урон: "), shooterAttackMinInput->Render() }),
            hbox({ text("Макс. урон: "), shooterAttackMaxInput->Render() }),
            hbox({ text("Мин. меткость: "), shooterAccuracyMinInput->Render() }),
            hbox({ text("Макс. меткость: "), shooterAccuracyMaxInput->Render() }),
            separator(),
            text("Характеристики защитников") | center | bold,
            separator(),
            hbox({ text("Мин. защита: "), defenderDefenseMinInput->Render() }),
            hbox({ text("Макс. защита: "), defenderDefenseMaxInput->Render() }),
            hbox({ text("Мин. реакция: "), defenderReactionMinInput->Render() }),
            hbox({ text("Макс. реакция: "), defenderReactionMaxInput->Render() })
        });

        auto rightCol = vbox({
            text("Состав команды") | center | bold,
            separator(),
            hbox({ text("Защитников: "), defendersCountInput->Render() }),
            hbox({ text("Стрелков: "), shootersCountInput->Render() }),
            hbox({ text("Стрелков на защитника: "), shootersPerDefenderInput->Render() }),
            separator(),
            text("Тайминги") | center | bold,
            separator(),
            hbox({ text("Задержка хода бота (мс): "), botDelayInput->Render() }),
            hbox({ text("Задержка атаки (мс): "), attackDelayInput->Render() })
        });

        return vbox({
                   text("НАСТРОЙКИ ИГРЫ") | bold | color(Config::UI::cyanColor) | center,
                   separator(),
                   hbox({ leftCol | flex, separator(), rightCol | flex }),
                   separator(),
                   backButton->Render() | center,
                   separator(),
                   text("Нажмите ESC для возврата в меню") | dim | center
               }) | border | center;
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape || event == Event::Character('q') || event == Event::CtrlC) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(component);
    currentScreen = 0;
}
