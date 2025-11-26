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
    auto backButton = Button("Назад", [&] { screen.Exit(); }, Config::UI::getMenuButtonStyle());
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

    std::vector<std::string> winConditionOptions = {"1", "2", "3", "4", "5"};
    std::vector<std::string> defendersCountOptions = {"1", "2", "3"};
    std::vector<std::string> shootersCountOptions = {"1", "2", "3", "4", "5", "6"};
    std::vector<std::string> shootersPerDefenderOptions = {"1", "2", "3"};
    std::vector<std::string> enableShootoutOptions = {"Вкл", "Выкл"};
    std::vector<std::string> enableCircusOptions = {"Вкл", "Выкл"};

    int winConditionSelected = Config::Game::winCondition - 1;
    int defendersCountSelected = Config::Team::defendersCount - 1;
    int shootersCountSelected = Config::Team::shootersCount - 1;
    int shootersPerDefenderSelected = Config::Defense::shootersPerDefender - 1;
    int enableShootoutSelected = Config::Game::enableShootout ? 0 : 1;
    int enableCircusSelected = Config::Game::enableCircus ? 0 : 1;

    auto winConditionToggle = Toggle(&winConditionOptions, &winConditionSelected);
    auto defendersCountToggle = Toggle(&defendersCountOptions, &defendersCountSelected);
    auto shootersCountToggle = Toggle(&shootersCountOptions, &shootersCountSelected);
    auto shootersPerDefenderToggle = Toggle(&shootersPerDefenderOptions, &shootersPerDefenderSelected);
    auto enableShootoutToggle = Toggle(&enableShootoutOptions, &enableShootoutSelected);
    auto enableCircusToggle = Toggle(&enableCircusOptions, &enableCircusSelected);

    int aggressiveAttackBonus = std::round((Config::Tactic::aggressiveAttackMultiplier - 1.0f) * 100);
    int aggressiveFatigueBonus = std::round((Config::Tactic::aggressiveFatigueMultiplier - 1.0f) * 100);
    int cautiousDefenseBonus = std::round((Config::Tactic::cautiousDefenseMultiplier - 1.0f) * 100);
    int cautiousAttackPenalty = std::round((1.0f - Config::Tactic::cautiousAttackMultiplier) * 100);

    std::string aggressiveAttackBonusStr = std::to_string(aggressiveAttackBonus);
    std::string aggressiveFatigueBonusStr = std::to_string(aggressiveFatigueBonus);
    std::string cautiousDefenseBonusStr = std::to_string(cautiousDefenseBonus);
    std::string cautiousAttackPenaltyStr = std::to_string(cautiousAttackPenalty);

    auto aggressiveAttackBonusInput = Input(&aggressiveAttackBonusStr, "15");
    auto aggressiveFatigueBonusInput = Input(&aggressiveFatigueBonusStr, "20");
    auto cautiousDefenseBonusInput = Input(&cautiousDefenseBonusStr, "15");
    auto cautiousAttackPenaltyInput = Input(&cautiousAttackPenaltyStr, "10");

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

    auto backButton = Button("Сохранить и выйти", [&] {
        Config::Game::winCondition = winConditionSelected + 1;
        Config::Team::defendersCount = defendersCountSelected + 1;
        Config::Team::shootersCount = shootersCountSelected + 1;
        Config::Defense::shootersPerDefender = shootersPerDefenderSelected + 1;
        Config::Game::enableShootout = (enableShootoutSelected == 0);
        Config::Game::enableCircus = (enableCircusSelected == 0);

        Config::Tactic::aggressiveAttackMultiplier = 1.0f + std::max(0.0f, std::stof(aggressiveAttackBonusStr) / 100.0f);
        Config::Tactic::aggressiveFatigueMultiplier = 1.0f + std::max(0.0f, std::stof(aggressiveFatigueBonusStr) / 100.0f);
        Config::Tactic::cautiousDefenseMultiplier = 1.0f + std::max(0.0f, std::stof(cautiousDefenseBonusStr) / 100.0f);
        Config::Tactic::cautiousAttackMultiplier = 1.0f - std::max(0.0f, std::min(1.0f, std::stof(cautiousAttackPenaltyStr) / 100.0f));

        Config::GasterGeneration::shooterAttackMin = std::max(1, std::stoi(shooterAttackMinStr));
        Config::GasterGeneration::shooterAttackMax = std::max(Config::GasterGeneration::shooterAttackMin, std::stoi(shooterAttackMaxStr));
        Config::GasterGeneration::shooterAccuracyMin = std::max(1, std::min(99, std::stoi(shooterAccuracyMinStr)));
        Config::GasterGeneration::shooterAccuracyMax = std::max(Config::GasterGeneration::shooterAccuracyMin, std::min(100, std::stoi(shooterAccuracyMaxStr)));
        Config::GasterGeneration::defenderDefenseMin = std::max(1, std::stoi(defenderDefenseMinStr));
        Config::GasterGeneration::defenderDefenseMax = std::max(Config::GasterGeneration::defenderDefenseMin, std::stoi(defenderDefenseMaxStr));
        Config::GasterGeneration::defenderReactionMin = std::max(1, std::min(99, std::stoi(defenderReactionMinStr)));
        Config::GasterGeneration::defenderReactionMax = std::max(Config::GasterGeneration::defenderReactionMin, std::min(100, std::stoi(defenderReactionMaxStr)));
        Config::Game::botMoveDelayMs = std::max(0, std::stoi(botDelayStr));
        Config::Game::attackDelayMs = std::max(0, std::stoi(attackDelayStr));

        screen.Exit();
    }, Config::UI::getMenuButtonStyle());

    auto leftColumn = Container::Vertical({
        winConditionToggle,
        defendersCountToggle,
        shootersCountToggle,
        shootersPerDefenderToggle,
        enableShootoutToggle,
        enableCircusToggle,
        aggressiveAttackBonusInput,
        aggressiveFatigueBonusInput,
        cautiousDefenseBonusInput,
        cautiousAttackPenaltyInput
    });

    auto rightColumn = Container::Vertical({
        shooterAttackMinInput,
        shooterAttackMaxInput,
        shooterAccuracyMinInput,
        shooterAccuracyMaxInput,
        defenderDefenseMinInput,
        defenderDefenseMaxInput,
        defenderReactionMinInput,
        defenderReactionMaxInput,
        botDelayInput,
        attackDelayInput
    });

    auto container = Container::Vertical({
        Container::Horizontal({ leftColumn, rightColumn }),
        backButton
    });

    auto renderer = Renderer(container, [&] {
        auto leftCol = vbox({
            text("Основные настройки") | center | bold,
            separator(),
            hbox({ text("Побед для победы: "), winConditionToggle->Render() }),
            hbox({ text("Кол-во защитников: "), defendersCountToggle->Render() }),
            hbox({ text("Кол-во стрелков: "), shootersCountToggle->Render() }),
            hbox({ text("Стрелков на защитника: "), shootersPerDefenderToggle->Render() }),
            hbox({ text("Решающая битва: "), enableShootoutToggle->Render() }),
            hbox({ text("Куча мола(beta): "), enableCircusToggle->Render() }),
            separator(),
            text("НАСТРОЙКИ ТАКТИК") | center | bold,
            separator(),
            hbox({ text("Агрессивная т.: атак. +"), aggressiveAttackBonusInput->Render(), text("%") }),
            hbox({ text("Агрессивная т.: уст. +"), aggressiveFatigueBonusInput->Render(), text("%") }),
            hbox({ text("Осторожная т.: защ. +"), cautiousDefenseBonusInput->Render(), text("%") }),
            hbox({ text("Осторожная т.: атак. -"), cautiousAttackPenaltyInput->Render(), text("%") })
        });

        auto rightCol = vbox({
            text("Характеристики персонажей") | center | bold,
            separator(),
            hbox({ text("Мин. урон стрелка: "), shooterAttackMinInput->Render() }),
            hbox({ text("Макс. урон стрелка: "), shooterAttackMaxInput->Render() }),
            hbox({ text("Мин. меткость стрелка: "), shooterAccuracyMinInput->Render() }),
            hbox({ text("Макс. меткость стрелка: "), shooterAccuracyMaxInput->Render() }),
            hbox({ text("Мин. защита защитника: "), defenderDefenseMinInput->Render() }),
            hbox({ text("Макс. защита защитника: "), defenderDefenseMaxInput->Render() }),
            hbox({ text("Мин. реакция защитника: "), defenderReactionMinInput->Render() }),
            hbox({ text("Макс. реакция защитника: "), defenderReactionMaxInput->Render() }),
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
