#include "shooter.h"
#include <iostream>

Shooter::Shooter(const std::string& name, int endurance, int attack, int accuracy)
    : Gaster(name, "Стрелок", endurance), attackPower(attack), accuracyValue(accuracy) {
    saveInitialStats(attack, accuracy, 0, 0);
    updateDerivedStats();
}

int Shooter::getAttack() const { return currentAttack; }
int Shooter::getAccuracy() const { return currentAccuracy; }
int Shooter::getPrimaryStat() const { return attackPower; }
int Shooter::getSecondaryStat() const { return accuracyValue; }
std::string Shooter::getType() const { return "shooter"; }

void Shooter::setAttackWithUpdate(int value) {
    attackPower = value;
    updateDerivedStats();
}

void Shooter::setAttack(int attack) {
    initialAttack = attack;
    attackPower = attack;
}

void Shooter::setAccuracy(int accuracy) {
    initialAccuracy = accuracy;
    accuracyValue = accuracy;
}

void Shooter::resetTacticEffects() {
    Gaster::resetTacticEffects();
    attackPower = initialAttack;
    accuracyValue = initialAccuracy;
    updateDerivedStats();
}

void Shooter::updateDerivedStats() {
    currentAttack = attackPower * currentAttackMultiplier;
    currentAccuracy = accuracyValue * currentAttackMultiplier;
    currentAttack = std::max(1, currentAttack);
    currentAccuracy = std::clamp(currentAccuracy, 1, 100);
}

bool Shooter::canBeSelected(const std::string& selector, int currentCount) {
    if (selectedByGaster != "") return false;

    int maxShooters = Config::Team::shootersCount;
    return currentCount < maxShooters;
}

Element Shooter::renderCard(bool isSelected, bool isFocused, bool isAttacker,
                            bool showProtection, int protectedBy, const std::string& currentTactic, bool isTacticPhase) {
    std::vector<Element> cardElements;

    int currentAttack = getAttack();
    int currentAccuracy = getAccuracy();
    int baseAttack = getOriginalAttack();
    int baseAccuracy = getOriginalAccuracy();

    if (selectedByGaster != "" && !isAttacker && !showProtection) {
        std::string selectedText = (selectedByGaster == "player") ? " (Твой)" : " (Бот)";
        Color borderColor = (selectedByGaster == "player") ? Config::UI::playerColor : Config::UI::botColor;

        cardElements.push_back(text(gasterName + selectedText) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());

        cardElements.push_back(hbox({
            text("Урон: "),
            text(std::to_string(currentAttack)) | color(Color::Red)
        }));

        cardElements.push_back(hbox({
            text("Меткость: "),
            text(std::to_string(currentAccuracy) + "%") | color(Color::GreenYellow)
        }));

        Color enduranceColor = isAlive ? Color::Green : Color::Red;
        auto enduranceGauge = hbox({
                                  text(std::to_string(gasterEndurance) + "% ") | size(WIDTH, EQUAL, 5),
                                  gauge(gasterEndurance / 100.0)
                              }) | color(enduranceColor);

        cardElements.push_back(enduranceGauge);

        if (currentTactic == "Агрессивная") {
            cardElements.push_back(text("Усталость +20%") | color(Color::Yellow) | dim);
        }

        auto element = vbox(cardElements) | flex;
        return element | borderDouble | color(borderColor);
    }
    else {
        cardElements.push_back(text(gasterName) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());

        if (isTacticPhase && (currentTactic == "Агрессивная" || currentTactic == "Осторожная")) {
            cardElements.push_back(hbox({
                text("Урон: "),
                text(std::to_string(baseAttack)) | color(Color::Red) | dim,
                text(" → ") | dim,
                text(std::to_string(currentAttack)) | color(Color::Red)
            }));
        } else if (!isTacticPhase && (currentTactic == "Агрессивная" || currentTactic == "Осторожная")) {
            cardElements.push_back(hbox({
                text("Урон: "),
                text(std::to_string(currentAttack)) | color(Color::Red)
            }));
        } else {
            cardElements.push_back(hbox({
                text("Урон: "),
                text(std::to_string(currentAttack)) | color(Color::Red)
            }));
        }

        if (isTacticPhase && (currentTactic == "Агрессивная" || currentTactic == "Осторожная")) {
            cardElements.push_back(hbox({
                text("Меткость: "),
                text(std::to_string(baseAccuracy)) | color(Color::GreenYellow) | dim,
                text(" → ") | dim,
                text(std::to_string(currentAccuracy) + "%") | color(Color::GreenYellow)
            }));
        } else if (!isTacticPhase && (currentTactic == "Агрессивная" || currentTactic == "Осторожная")) {
            cardElements.push_back(hbox({
                text("Меткость: "),
                text(std::to_string(currentAccuracy) + "%") | color(Color::GreenYellow)
            }));
        } else {
            cardElements.push_back(hbox({
                text("Меткость: "),
                text(std::to_string(currentAccuracy) + "%") | color(Color::GreenYellow)
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
        } else {
            element = element | border;
        }

        return element;
    }
}
