#include "defender.h"
#include <iostream>

Defender::Defender(const std::string& name, int endurance, int defense, int reaction)
    : Gaster(name, "Защитник", endurance), defensePower(defense), reactionValue(reaction) {
    saveInitialStats(0, 0, defense, reaction);
    updateDerivedStats();
}

int Defender::getDefense() const { return currentDefense; }
int Defender::getReaction() const { return currentReaction; }
int Defender::getPrimaryStat() const { return defensePower; }
int Defender::getSecondaryStat() const { return reactionValue; }
std::string Defender::getType() const { return "defender"; }

bool Defender::canBeSelected(const std::string& selector, int currentCount) {
    if (selectedByGaster != "") return false;
    int maxDefenders = Config::Team::defendersCount;
    return currentCount < maxDefenders;
}

void Defender::setDefenseWithUpdate(int value) {
    defensePower = value;
    updateDerivedStats();
}

void Defender::setDefense(int defender) {
    initialDefense = defender;
    defensePower = defender;
}

void Defender::setReaction(int reaction) {
    initialReaction = reaction;
    reactionValue = reaction;
}

void Defender::resetTacticEffects() {
    Gaster::resetTacticEffects();
    defensePower = initialDefense;
    reactionValue = initialReaction;
    updateDerivedStats();
}

void Defender::updateDerivedStats() {
    currentDefense = defensePower * currentDefenseMultiplier;
    currentReaction = reactionValue * currentDefenseMultiplier;
    currentDefense = std::max(1, currentDefense);
    currentReaction = std::clamp(currentReaction, 1, 100);
}

Element Defender::renderCard(bool isSelected, bool isFocused, bool isAttacker,
                             bool showProtection, int protectedBy, const std::string& currentTactic, bool isTacticPhase) {
    std::vector<Element> cardElements;
    int currentDefense = getDefense();
    int currentReaction = getReaction();
    int baseDefense = getOriginalDefense();
    int baseReaction = getOriginalReaction();

    if (selectedByGaster != "" && !isAttacker && !showProtection) {
        std::string selectedText = (selectedByGaster == "player") ? " (Твой)" : " (Бот)";
        Color borderColor = (selectedByGaster == "player") ? Config::UI::playerColor : Config::UI::botColor;

        cardElements.push_back(text(gasterName + selectedText) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());

        cardElements.push_back(hbox({
            text("Защита: "),
            text(std::to_string(currentDefense)) | color(Color::Blue)
        }));

        cardElements.push_back(hbox({
            text("Реакция: "),
            text(std::to_string(currentReaction) + "%") | color(Color::GreenYellow)
        }));

        if (!protectedShooterIndicesList.empty()) {
            cardElements.push_back(hbox({
                text("Защищает: "),
                text(std::to_string(protectedShooterIndicesList.size()) + " стр.") | color(Color::Green)
            }));
        }

        Color enduranceColor = isAlive ? Color::Green : Color::Red;
        auto enduranceGauge = hbox({
                                  text(std::to_string(gasterEndurance) + "% ") | size(WIDTH, EQUAL, 5),
                                  gauge(gasterEndurance / 100.0)
                              }) | color(enduranceColor);

        cardElements.push_back(enduranceGauge);

        auto element = vbox(cardElements) | flex;
        return element | borderDouble | color(borderColor);
    }
    else {
        cardElements.push_back(text(gasterName) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());

        if (isTacticPhase && currentTactic == "Осторожная") {
            cardElements.push_back(hbox({
                text("Защита: "),
                text(std::to_string(baseDefense)) | color(Color::Blue) | dim,
                text(" → ") | dim,
                text(std::to_string(currentDefense)) | color(Color::Blue)
            }));
        } else if (!isTacticPhase && (currentTactic == "Осторожная" || currentTactic == "Агрессивная")) {
            cardElements.push_back(hbox({
                text("Защита: "),
                text(std::to_string(currentDefense)) | color(Color::Blue)
            }));
        } else {
            cardElements.push_back(hbox({
                text("Защита: "),
                text(std::to_string(currentDefense)) | color(Color::Blue)
            }));
        }

        if (isTacticPhase && currentTactic == "Осторожная") {
            cardElements.push_back(hbox({
                text("Реакция: "),
                text(std::to_string(baseReaction)) | color(Color::GreenYellow) | dim,
                text(" → ") | dim,
                text(std::to_string(currentReaction) + "%") | color(Color::GreenYellow)
            }));
        } else if (!isTacticPhase && (currentTactic == "Осторожная" || currentTactic == "Агрессивная")) {
            cardElements.push_back(hbox({
                text("Реакция: "),
                text(std::to_string(currentReaction) + "%") | color(Color::GreenYellow)
            }));
        } else {
            cardElements.push_back(hbox({
                text("Реакция: "),
                text(std::to_string(currentReaction) + "%") | color(Color::GreenYellow)
            }));
        }

        Color enduranceColor = isAlive ? Color::Green : Color::Red;
        auto enduranceGauge = hbox({
                                  text(std::to_string(gasterEndurance) + "% ") | size(WIDTH, EQUAL, 5),
                                  gauge(gasterEndurance / 100.0)
                              }) | color(enduranceColor);

        cardElements.push_back(enduranceGauge);

        auto element = vbox(cardElements) | flex;

        if (isSelected) {
            element = isAttacker ? element | borderDouble | color(Color::Green) : element | borderDouble | color(Color::Red);
        } else if (isFocused) {
            element = element | borderDouble | color(Color::White);
        } else if (!isAlive) {
            element = element | border | color(Color::Red);
        } else if (gasterRole != "Стрелок" && isAlive) {
            element = element | border | color(Color::GrayDark);
        } else {
            element = element | border;
        }

        return element;
    }
}
