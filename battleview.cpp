#include "battleview.h"

BattleView::BattleView(BattleController& controller)
    : controller(controller) {}

Element BattleView::renderPlayerInfo(int index, Team& team, const std::string& title, Color titleColor, bool isPlayerTeam) {
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

    int baseAttack = player->getAttack();
    int baseAccuracy = player->getAccuracy();
    int baseDefense = player->getDefense();
    int baseReaction = player->getReaction();

    int currentAttack = baseAttack;
    int currentAccuracy = baseAccuracy;
    int currentDefense = baseDefense;
    int currentReaction = baseReaction;

    if (player->getAttack() > 0) {
        if (controller.isWaitingForTactic() && isPlayerTeam) {
            elements.push_back(hbox({
                text("Урон: "),
                text(std::to_string(baseAttack)) | color(Color::Red) | dim,
                text(" → "),
                text(std::to_string(currentAttack)) | color(Color::Red) | bold
            }));
        } else {
            elements.push_back(hbox({ text("Урон: "), text(std::to_string(currentAttack)) | color(Color::Red) }));
        }
    }

    if (player->getAccuracy() > 0) {
        if (controller.isWaitingForTactic() && isPlayerTeam) {
            elements.push_back(hbox({
                text("Меткость: "),
                text(std::to_string(baseAccuracy)) | color(Color::GreenYellow) | dim,
                text(" → "),
                text(std::to_string(currentAccuracy) + "%") | color(Color::GreenYellow) | bold
            }));
        } else {
            elements.push_back(hbox({ text("Меткость: "), text(std::to_string(currentAccuracy) + "%") | color(Color::GreenYellow) }));
        }
    }

    if (player->getDefense() > 0) {
        if (controller.isWaitingForTactic() && isPlayerTeam) {
            elements.push_back(hbox({
                text("Защита: "),
                text(std::to_string(baseDefense)) | color(Config::UI::defenderColor) | dim,
                text(" → "),
                text(std::to_string(currentDefense)) | color(Config::UI::defenderColor) | bold
            }));
        } else {
            elements.push_back(hbox({ text("Защита: "), text(std::to_string(currentDefense)) | color(Config::UI::defenderColor) }));
        }
    }

    if (player->getReaction() > 0) {
        if (controller.isWaitingForTactic() && isPlayerTeam) {
            elements.push_back(hbox({
                text("Реакция: "),
                text(std::to_string(baseReaction)) | color(Color::GreenYellow) | dim,
                text(" → "),
                text(std::to_string(currentReaction) + "%") | color(Color::GreenYellow) | bold
            }));
        } else {
            elements.push_back(hbox({ text("Реакция: "), text(std::to_string(currentReaction) + "%") | color(Color::GreenYellow) }));
        }
    }

    return vbox(elements);
}

void BattleView::run() {
    auto screen = ScreenInteractive::Fullscreen();
    auto leftButtons = createTeamButtons(controller.getPlayerTeam(), true);
    auto rightButtons = createTeamButtons(controller.getBotTeam(), false);

    auto leftContainer = createTeamContainer(controller.getPlayerTeam(), leftButtons,
                                             "ВАША КОМАНДА",
                                             Config::UI::playerColor, false, true);
    auto rightContainer = createTeamContainer(controller.getBotTeam(), rightButtons,
                                              "КОМАНДА ПРОТИВНИКА",
                                              Color::Yellow, true, false);

    std::string currentMessage = "";
    auto messageInput = Input(&currentMessage, "Напишите сообщение...");
    auto sendButton = Button("Отправить", [&] {
        controller.sendMessage(currentMessage);
        currentMessage.clear();
    }, Config::UI::getMenuButtonStyle());
    auto attackButton = Button("Атаковать", [&] { controller.performPlayerAttack(); }, Config::UI::getMenuButtonStyle());

    auto aggressiveTacticButton = Button("Агрессивная", [this] {
        controller.selectPlayerTactic("Агрессивная");
    }, Config::UI::getMenuButtonStyle());
    auto cautiousTacticButton = Button("Осторожная", [this] {
        controller.selectPlayerTactic("Осторожная");
    }, Config::UI::getMenuButtonStyle());
    auto normalTacticButton = Button("Обычный", [this] {
        controller.selectPlayerTactic("Обычный");
    }, Config::UI::getMenuButtonStyle());

    auto confirmTacticButton = Button("Подтвердить тактику", [this] {
        controller.confirmTactic();
    }, Config::UI::getMenuButtonStyle());

    auto tacticContainer = Container::Horizontal({
        aggressiveTacticButton,
        cautiousTacticButton,
        normalTacticButton,
        confirmTacticButton
    });

    auto chatComponent = controller.getChatManager().createChatComponent();

    auto centerContainer = Container::Vertical({
        chatComponent,
        Container::Horizontal({ messageInput, sendButton, attackButton }),
        tacticContainer
    });

    auto centerRenderer = Renderer(centerContainer, [this, aggressiveTacticButton, cautiousTacticButton, normalTacticButton, confirmTacticButton, &messageInput, &sendButton, &attackButton, &chatComponent]() {
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

        if (controller.isWaitingForTactic()) {
            elements.push_back(renderTacticSelection(aggressiveTacticButton, cautiousTacticButton, normalTacticButton, confirmTacticButton));
            elements.push_back(separator());
        }

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
    });

    int leftSize = 45, rightSize = 45;
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

                    std::string currentTactic = isPlayerTeam ?
                                                     (controller.isWaitingForTactic() ?
                                                          controller.getPendingPlayerTactic() :
                                                          controller.getPlayerTactic()) :
                                                     controller.getBotTactic();
                    bool isTacticPhase = controller.isWaitingForTactic();

                    return team.getPlayer(i)->renderCard(
                        (isPlayerTeam && controller.getSelectedAttacker() == i) ||
                            (!isPlayerTeam && controller.getSelectedTarget() == i),
                        state.focused,
                        isPlayerTeam,
                        showProtection,
                        protectedBy,
                        currentTactic,
                        isTacticPhase
                        );
                };
                return opt;
            }(ButtonOption::Ascii())
            ));
    }
    return buttons;
}

Component BattleView::createTeamContainer(Team& team, std::vector<Component>& buttons, const std::string& title, Color titleColor, bool reverseOrder, bool isPlayerTeam) {
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

        std::string tacticInfo = "";
        if (isPlayerTeam) {
            tacticInfo = controller.isWaitingForTactic() ?
                             "" :
                             " (" + controller.getPlayerTactic() + ")";
        } else {
            tacticInfo = controller.isWaitingForTactic()?
                             "" :
                             " (" + controller.getBotTactic() + ")";
        }

        elements.push_back(text(title + tacticInfo) | bold | color(titleColor) | center);
        elements.push_back(separator());

        std::string Info;
        int aggFatigue = std::round((Config::Tactic::aggressiveFatigueMultiplier - 1.0f) * 100);

        if (isPlayerTeam && controller.getPlayerTactic() == "Агрессивная") {
            Info = " (Уст. +" + std::to_string(aggFatigue) +"%)";
        } else if (!isPlayerTeam && controller.getBotTactic() == "Агрессивная") {
            Info = " (Уст. +" + std::to_string(aggFatigue) +"%)";
        }
        std::vector<Element> shootersElements = {
            text("СТРЕЛКИ" + Info) | bold | color(Config::UI::shooterColor) | center,
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

Element BattleView::renderSelectionInfo() {
    std::vector<Element> selectionElements;

    Element attackerInfo = renderPlayerInfo(controller.getSelectedAttacker(), controller.getPlayerTeam(),
                                            "АТАКУЮЩИЙ", Config::UI::playerColor, true);
    Element targetInfo = renderPlayerInfo(controller.getSelectedTarget(), controller.getBotTeam(),
                                          "ЦЕЛЬ", Config::UI::botColor, false);

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

Element BattleView::renderInstructions() {
    if (controller.isWaitingForTactic()) {
        return vbox({
            text("СНАЧАЛА ВЫБЕРИТЕ ТАКТИКУ!") | bold | color(Color::Yellow) | center,
            text("Порядок действий:") | bold | center,
            text("1. Выберите тактику для раунда") | dim,
            text("2. Нажмите 'Подтвердить тактику'") | dim,
            text("3. Выберите стрелка из вашей команды") | dim,
            text("4. Выберите защитника из команды противника") | dim,
            text("5. Нажмите 'Атаковать'") | dim
        });
    } else {
        return vbox({
            text("ПОРЯДОК ДЕЙСТВИЙ:") | bold | center,
            text("1. Выбери стрелка из вашей команды") | dim,
            text("2. Выбери защитника из команды противника") | dim,
            text("3. Нажми 'Атаковать'") | dim
        });
    }
}

Element BattleView::renderTacticSelection(const Component& aggressiveBtn, const Component& cautiousBtn, const Component& normalBtn, const Component& confirmBtn) {
    std::string currentPendingTactic = controller.getPendingPlayerTactic();

    Element aggressiveElement = aggressiveBtn->Render();
    Element cautiousElement = cautiousBtn->Render();
    Element normalElement = normalBtn->Render();
    Element confirmElement = confirmBtn->Render();

    if (currentPendingTactic == "Агрессивная") {
        aggressiveElement = aggressiveElement | bgcolor(Color::DarkRed) | color(Color::White);
    } else if (currentPendingTactic == "Осторожная") {
        cautiousElement = cautiousElement | bgcolor(Color::DarkBlue) | color(Color::White);
    } else if (currentPendingTactic == "Обычный") {
        normalElement = normalElement | bgcolor(Color::DarkGreen) | color(Color::White);
    }

    return vbox({
        hbox({
            aggressiveElement | flex,
            text(" ") | flex,
            cautiousElement | flex,
            text(" ") | flex,
            normalElement | flex,
            text(" ") | flex,
            confirmElement | flex
        }) | center
    });
}
