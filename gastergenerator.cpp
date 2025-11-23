#include "gastergenerator.h"

std::unique_ptr<Gaster> GasterGenerator::createShooter(std::string& name) {
    int attack = Config::randomInRange(
        Config::GasterGeneration::shooterAttackMin,
        Config::GasterGeneration::shooterAttackMax
        );
    int accuracy = Config::randomInRange(
        Config::GasterGeneration::shooterAccuracyMin,
        Config::GasterGeneration::shooterAccuracyMax
        );
    return std::make_unique<Shooter>(name, Config::GasterGeneration::endurance, attack, accuracy);
}

std::unique_ptr<Gaster> GasterGenerator::createDefender(std::string& name) {
    int defense = Config::randomInRange(
        Config::GasterGeneration::defenderDefenseMin,
        Config::GasterGeneration::defenderDefenseMax
        );
    int reaction = Config::randomInRange(
        Config::GasterGeneration::defenderReactionMin,
        Config::GasterGeneration::defenderReactionMax
        );
    return std::make_unique<Defender>(name, Config::GasterGeneration::endurance, defense, reaction);
}

std::vector<std::unique_ptr<Gaster>> GasterGenerator::generateAllPlayers() {
    std::vector<std::unique_ptr<Gaster>> players;

    std::vector<std::string> defenderNames = {
        "Бульдозер", "Скала", "Громила", "Танк", "Бастион",
        "Щит", "Стена", "Крепость", "Дозорный", "Защитник"
    };

    std::vector<std::string> shooterNames = {
        "Призрак", "Снайпер", "Тень", "Молот", "Ворон",
        "Клинок", "Рейдер", "Хищник", "Стрелок", "Меткий",
        "Охотник", "Сокол", "Беркут", "Ястреб", "Орёл"
    };

    int totalDefendersNeeded = std::min(3, Config::Team::defendersCount) * 2;
    int totalShootersNeeded = std::min(6, Config::Team::shootersCount) * 2;

    for (int i = 0; i < totalDefendersNeeded; i++) {
        std::string name;
        if (i < defenderNames.size()) {
            name = defenderNames[i];
        } else {
            name = "Защитник_" + std::to_string(i + 1);
        }
        players.push_back(createDefender(name));
    }

    for (int i = 0; i < totalShootersNeeded; i++) {
        std::string name;
        if (i < shooterNames.size()) {
            name = shooterNames[i];
        } else {
            name = "Стрелок_" + std::to_string(i + 1);
        }
        players.push_back(createShooter(name));
    }

    return players;
}
