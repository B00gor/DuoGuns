#ifndef CONFIG_H
#define CONFIG_H

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <random>

using namespace ftxui;

inline std::mt19937 rng{std::random_device{}()};

namespace Config {
namespace GasterGeneration {
inline int shooterAttackMin = 50;
inline int shooterAttackMax = 90;
inline int shooterAccuracyMin = 70;
inline int shooterAccuracyMax = 95;
inline int defenderDefenseMin = 60;
inline int defenderDefenseMax = 100;
inline int defenderReactionMin = 65;
inline int defenderReactionMax = 90;
inline int endurance = 100;
}

namespace Team {
inline int defendersCount = 2;
inline int shootersCount = 4;
inline int totalPlayers() { return defendersCount + shootersCount; }
}

namespace Game {
inline int winCondition = 5;
inline int botMoveDelayMs = 100;
inline int messageDelayMs = 500;
inline int attackDelayMs = 200;
inline bool enableShootout = true;
inline bool enableCircus = false;
}

namespace UI {
inline Color playerColor = Color::Green;
inline Color botColor = Color::Red;
inline Color shooterColor = Color::Red;
inline Color defenderColor = Color::Blue;
inline Color systemColor = Color::Yellow;
inline Color cyanColor = Color::Cyan;
inline Color dimColor = Color::GrayDark;

inline auto getMenuButtonStyle() {
    auto style = ButtonOption::Ascii();
    style.transform = [](const EntryState& state) {
        auto element = text(state.label) | center | flex;
        if (state.focused) {
            element = element | borderDouble | color(Color::White);
        } else {
            element = element | border;
        }
        return element;
    };
    return style;
}

inline auto getMenuButtonStyleError() {
    auto style = ButtonOption::Ascii();
    style.transform = [](const EntryState& state) {
        auto element = text(state.label) | center | flex;
        if (state.focused) {
            element = element | borderDouble | strikethrough | color(Color::Red);
        } else {
            element = element | strikethrough | border;
        }
        return element;
    };
    return style;
}
}

namespace Defense {
inline int shootersPerDefender = 2;
}

namespace Tactic {
inline float aggressiveAttackMultiplier = 1.15f;
inline float aggressiveDefenseMultiplier = 1.0f;
inline float aggressiveFatigueMultiplier = 1.2f;

inline float cautiousAttackMultiplier = 0.9f;
inline float cautiousDefenseMultiplier = 1.15f;
inline float cautiousFatigueMultiplier = 1.0f;

inline float normalAttackMultiplier = 1.0f;
inline float normalDefenseMultiplier = 1.0f;
inline float normalFatigueMultiplier = 1.0f;
}

inline int randomInRange(int min, int max) {
    return std::uniform_int_distribution<int>{min, max}(rng);
}
}

#endif // CONFIG_H
