#include "battleview.h"

BattleView::BattleView(BattleController& controller)
    : controller(controller) {}

void BattleView::run() {
    auto screen = ScreenInteractive::Fullscreen();

    auto leftButtons = createTeamButtons(controller.getPlayerTeam(), true);
    auto rightButtons = createTeamButtons(controller.getBotTeam(), false);

    auto leftContainer = createTeamContainer(controller.getPlayerTeam(), leftButtons,
                                             "ВАША КОМАНДА", Config::UI::playerColor, false);
    auto rightContainer = createTeamContainer(controller.getBotTeam(), rightButtons,
                                              "КОМАНДА ПРОТИВНИКА", Color::Yellow, true);

    std::string currentMessage = "";
    auto messageInput = Input(&currentMessage, "Напишите сообщение...");
    auto sendButton = Button("Отправить", [&] {
        controller.sendMessage(currentMessage);
        currentMessage.clear();
    });
    auto attackButton = Button("Атаковать", [&] { controller.performPlayerAttack(); });

    auto chatComponent = controller.getChatManager().createChatComponent();

    auto centerContainer = Container::Vertical({
        chatComponent,
        Container::Horizontal({ messageInput, sendButton, attackButton })
    });

    auto centerRenderer = Renderer(centerContainer, [&] {
        return renderCenterPanel(chatComponent, messageInput, sendButton, attackButton);
    });

    int leftSize = 40, rightSize = 40;
    auto container = centerRenderer;
    container = ResizableSplitLeft(leftContainer, container, &leftSize);
    container = ResizableSplitRight(rightContainer, container, &rightSize);

    auto exitButtonStyle = ButtonOption::Ascii();
    exitButtonStyle.transform = [](const EntryState& state) {
        auto element = text(state.label);
        if (state.focused) element = element | color(Color::Red) | bold;
        return element;
    };
    auto exitButton = Button("Выход", [&] { screen.Exit(); }, exitButtonStyle);

    auto topContainer = Container::Horizontal({ exitButton });

    auto mainContainer = Container::Vertical({
        topContainer,
        container
    });

    auto mainRenderer = Renderer(mainContainer, [&] {
        return vbox({
                   hbox({
                       exitButton->Render(),
                       text("БОЕВАЯ СИСТЕМА") | bold | center | flex
                   }),
                   separator(),
                   container->Render() | flex
               }) | flex;
    });

    auto gameComponent = CatchEvent(mainRenderer, [&](Event event) {
        if (event == Event::Escape || event == Event::Character('q') || event == Event::CtrlC) {
            screen.Exit();
            return true;
        }
        if (event == Event::Return && !currentMessage.empty()) {
            controller.sendMessage(currentMessage);
            currentMessage.clear();
            return true;
        }
        return false;
    });

    screen.Loop(gameComponent);
}


std::vector<Component> BattleView::createTeamButtons(Team& team, bool isPlayerTeam) {
    std::vector<Component> buttons;
    for (int i = 0; i < team.size(); i++) {
        buttons.push_back(Button(
            "",
            [this, i, isPlayerTeam] {
                if (isPlayerTeam)
                    controller.handleAttackerSelection(i);
                else
                    controller.handleTargetSelection(i);
            },
            [this, i, isPlayerTeam, &team](const ButtonOption& defaultOption) {
                auto opt = defaultOption;
                opt.transform = [this, i, isPlayerTeam, &team](const EntryState& state) {
                    bool showProtection = !isPlayerTeam;
                    int protectedBy = -1;
                    if (showProtection) {
                        protectedBy = controller.getGameEngine().defenseSystem().getDefenderForShooter(team, i);
                    }
                    return team.getPlayer(i)->renderCard(
                        (isPlayerTeam && controller.getSelectedAttacker() == i) ||
                            (!isPlayerTeam && controller.getSelectedTarget() == i),
                        state.focused,
                        isPlayerTeam,
                        showProtection,
                        protectedBy
                        );
                };
                return opt;
            }(ButtonOption::Ascii())
            ));
    }
    return buttons;
}

Component BattleView::createTeamContainer(Team& team, std::vector<Component>& buttons, const std::string& title, Color titleColor, bool reverseOrder) {
    auto shooters = Container::Vertical({});
    auto defenders = Container::Vertical({});

    auto shooterIndices = team.getShooterIndices();
    auto defenderIndices = team.getDefenderIndices();

    for (int index : shooterIndices) {
        shooters->Add(buttons[index]);
    }
    for (int index : defenderIndices) {
        defenders->Add(buttons[index]);
    }

    Component container;
    if (reverseOrder) {
        container = Container::Horizontal({ defenders, shooters });
    } else {
        container = Container::Horizontal({ shooters, defenders });
    }

    return Renderer(container, [=] {
        std::vector<Element> elements;
        elements.push_back(text(title) | bold | color(titleColor) | center);
        elements.push_back(separator());

        std::vector<Element> shootersElements = {
            text("СТРЕЛКИ") | bold | color(Config::UI::shooterColor) | center,
            shooters->Render() | flex
        };

        std::vector<Element> defendersElements = {
            text("ЗАЩИТНИКИ") | bold | color(Config::UI::defenderColor) | center,
            defenders->Render() | flex
        };

        if (reverseOrder) {
            elements.push_back(hbox({
                                   vbox(defendersElements) | flex,
                                   vbox(shootersElements) | flex
                               }) | flex);
        } else {
            elements.push_back(hbox({
                                   vbox(shootersElements) | flex,
                                   vbox(defendersElements) | flex
                               }) | flex);
        }

        return vbox(elements) | border;
    });
}

Element BattleView::renderCenterPanel(Component& chatComponent, Component& messageInput, Component& sendButton, Component& attackButton) {
    std::vector<Element> elements;

    elements.push_back(hbox({
        text("  " + std::to_string(controller.getPlayerWins())) | color(Config::UI::playerColor) | bold,
        text(" ") | flex,
        text("ЧАТ ИГРЫ") | bold | color(Config::UI::cyanColor),
        text(" ") | flex,
        text(std::to_string(controller.getBotWins()) + "  ") | color(Config::UI::botColor) | bold
    }));
    elements.push_back(separator());
    elements.push_back(chatComponent->Render() | flex);
    elements.push_back(separator());
    elements.push_back(hbox({
        messageInput->Render() | flex,
        sendButton->Render(),
        attackButton->Render()
    }));
    elements.push_back(separator());

    if (controller.getSelectedAttacker() != -1 || controller.getSelectedTarget() != -1) {
        elements.push_back(renderSelectionInfo());
    } else {
        elements.push_back(renderInstructions());
    }

    elements.push_back(separator());
    elements.push_back(text("Нажмите ESC для возврата в меню | Enter для отправки сообщения") | dim | center);

    return vbox(elements) | border;
}

Element BattleView::renderSelectionInfo() {
    std::vector<Element> selectionElements;

    Element attackerInfo = renderPlayerInfo(controller.getSelectedAttacker(), controller.getPlayerTeam(),
                                            "АТАКУЮЩИЙ", Config::UI::playerColor);
    Element targetInfo = renderPlayerInfo(controller.getSelectedTarget(), controller.getBotTeam(),
                                          "ЦЕЛЬ", Config::UI::botColor);

    selectionElements.push_back(hbox({ attackerInfo | flex, separator(), targetInfo | flex }) | flex);
    selectionElements.push_back(separator());

    if (controller.getSelectedAttacker() != -1 && controller.getSelectedTarget() != -1) {
        Gaster* target = controller.getBotTeam().getPlayer(controller.getSelectedTarget());
        if (target->role() == "Стрелок") {
            int defenderIndex = controller.getGameEngine().defenseSystem().getDefenderForShooter(
                controller.getBotTeam(), controller.getSelectedTarget());
            if (defenderIndex != -1 && controller.getBotTeam().getPlayer(defenderIndex)->alive()) {
                selectionElements.push_back(text("Невозможно атаковать! Сначала победите защитника!") |
                                            bold | color(Color::Red) | center);
            } else {
                selectionElements.push_back(text("Готов к атаке! Нажмите 'Атаковать'") |
                                            bold | color(Color::Green) | center);
            }
        } else {
            selectionElements.push_back(text("Готов к атаке! Нажмите 'Атаковать'") |
                                        bold | color(Color::Green) | center);
        }
    } else {
        selectionElements.push_back(text("Выберите атакующего и цель для атаки") | dim | center);
    }

    return vbox(selectionElements);
}

Element BattleView::renderPlayerInfo(int index, Team& team, const std::string& title, Color titleColor) {
    if (index == -1) {
        return vbox({
            text(title) | bold | color(titleColor) | center,
            separator(),
            text("НЕ ВЫБРАН") | dim | center
        });
    }

    Gaster* player = team.getPlayer(index);
    std::vector<Element> elements;
    elements.push_back(text(title) | bold | color(titleColor) | center);
    elements.push_back(separator());
    elements.push_back(text(player->name()) | bold | color(Color::Cyan) | center);
    elements.push_back(hbox({ text("Роль: "), text(player->role()) | color(
                                                 player->role() == "Стрелок" ? Config::UI::shooterColor :
                                                     Config::UI::defenderColor) | bold }));
    elements.push_back(hbox({ text("Выносл.: "), text(std::to_string(player->endurance()) + "%") | color(Color::Green) }));

    if (player->getAttack() > 0) {
        elements.push_back(hbox({ text("Урон: "), text(std::to_string(player->getAttack())) | color(Color::Red) }));
    }
    if (player->getAccuracy() > 0) {
        elements.push_back(hbox({ text("Меткость: "), text(std::to_string(player->getAccuracy()) + "%") | color(Color::GreenYellow) }));
    }
    if (player->getDefense() > 0) {
        elements.push_back(hbox({ text("Защита: "), text(std::to_string(player->getDefense())) | color(Config::UI::defenderColor) }));
    }
    if (player->getReaction() > 0) {
        elements.push_back(hbox({ text("Реакция: "), text(std::to_string(player->getReaction()) + "%") | color(Color::GreenYellow) }));
    }
    return vbox(elements);
}

Element BattleView::renderInstructions() {
    return vbox({
        text("ПОРЯДОК ДЕЙСТВИЙ:") | bold | center,
        text("1. Выбери стрелка из вашей команды") | dim,
        text("2. Выбери защитника из команды противника") | dim,
        text("3. Нажми 'Атаковать'") | dim
    });
}
