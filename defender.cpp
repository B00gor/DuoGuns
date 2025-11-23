#include "defender.h"

Defender::Defender(const std::string& name, int endurance, int defense, int reaction)
    : Gaster(name, "Защитник", endurance), defensePower(defense), reactionValue(reaction) {}

int Defender::getDefense()  { return defensePower; }
int Defender::getReaction()  { return reactionValue; }
int Defender::getPrimaryStat()  { return defensePower; }
int Defender::getSecondaryStat()  { return reactionValue; }
std::string Defender::getType()  { return "defender"; }

void Defender::setDefense(int defense) {
    defensePower = defense;
}

void Defender::setReaction(int reaction) {
    reactionValue = reaction;
}

Element Defender::renderCard(bool isSelected, bool isFocused, bool isAttacker,
                             bool showProtection, int protectedBy) {
    std::vector<Element> cardElements;

    if (selectedByGaster != "" && !isAttacker && !showProtection) {
        std::string selectedText = (selectedByGaster == "player") ? " (Твой)" : " (Бот)";
        Color borderColor = (selectedByGaster == "player") ? Config::UI::playerColor : Config::UI::botColor;

        cardElements.push_back(text(gasterName + selectedText) | bold | center | color(Color::Cyan));
        cardElements.push_back(separator());
        cardElements.push_back(hbox({ text("Защита: "), text(std::to_string(defensePower)) | color(Color::Blue) }));
        cardElements.push_back(hbox({ text("Реакция: "), text(std::to_string(reactionValue) + "%") | color(Color::GreenYellow) }));

        if (!protectedShooterIndicesList.empty()) {
            cardElements.push_back(hbox({
                text("Защищает: "),
                text(std::to_string(protectedShooterIndicesList.size()) + " стр.") | color(Color::Green)
            }));
        }

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
        cardElements.push_back(hbox({ text("Защита: "), text(std::to_string(defensePower)) | color(Color::Blue) }));
        cardElements.push_back(hbox({ text("Реакция: "), text(std::to_string(reactionValue) + "%") | color(Color::GreenYellow) }));

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
        } else if (gasterRole != "Стрелок" && isAlive) {
            element = element | border | color(Color::GrayDark);
        } else {
            element = element | border;
        }

        return element;
    }
}
