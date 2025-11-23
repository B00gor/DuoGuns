#include "shooter.h"

Shooter::Shooter(const std::string& name, int endurance, int attack, int accuracy)
    : Gaster(name, "Стрелок", endurance), attackPower(attack), accuracyValue(accuracy) {}

int Shooter::getAttack()  { return attackPower; }
int Shooter::getAccuracy()  { return accuracyValue; }
int Shooter::getPrimaryStat()  { return attackPower; }
int Shooter::getSecondaryStat()  { return accuracyValue; }
std::string Shooter::getType()  { return "shooter"; }

void Shooter::setAttack(int attack) {
    attackPower = attack;
}

void Shooter::setAccuracy(int accuracy) {
    accuracyValue = accuracy;
}

Element Shooter::renderCard(bool isSelected, bool isFocused, bool isAttacker,
                            bool showProtection, int protectedBy) {
    std::vector<Element> cardElements;

    if (selectedByGaster != "" && !isAttacker && !showProtection) {
        std::string selectedText = (selectedByGaster == "player") ? " (Твой)" : " (Бот)";
        Color borderColor = (selectedByGaster == "player") ? Config::UI::playerColor : Config::UI::botColor;

        cardElements.push_back(text(gasterName + selectedText) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());
        cardElements.push_back(hbox({ text("Урон: "), text(std::to_string(attackPower)) | color(Color::Red) }));
        cardElements.push_back(hbox({ text("Меткость: "), text(std::to_string(accuracyValue) + "%") | color(Color::GreenYellow) }));

        auto enduranceGauge = hbox({
                                  text(std::to_string(gasterEndurance) + "% ") | size(WIDTH, EQUAL, 5),
                                  gauge(gasterEndurance / 100.0)
                              }) | color(Color::Green);

        cardElements.push_back(enduranceGauge);

        auto element = vbox(cardElements) | flex;
        return element | borderDouble | color(borderColor);
    }
    else {
        cardElements.push_back(text(gasterName) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());
        cardElements.push_back(hbox({ text("Урон: "), text(std::to_string(attackPower)) | color(Color::Red) }));
        cardElements.push_back(hbox({ text("Меткость: "), text(std::to_string(accuracyValue) + "%") | color(Color::GreenYellow) }));

        Color enduranceColor = isAlive ? Color::Green : Color::Red;
        auto enduranceGauge = hbox({
                                  text(std::to_string(gasterEndurance) + "% ") | size(WIDTH, EQUAL, 5),
                                  gauge(gasterEndurance / 100.0)
                              }) | color(enduranceColor);

        cardElements.push_back(enduranceGauge);

        auto element = vbox(cardElements) | flex;

        if (isSelected) {
            element = isAttacker ?
                          element | borderDouble | color(Color::Green) :
                          element | borderDouble | color(Color::Red);
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
