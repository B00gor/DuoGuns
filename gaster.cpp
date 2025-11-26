#include "gaster.h"

Gaster::Gaster(const std::string& name, const std::string& role, int endurance)
    : gasterName(name), gasterRole(role), gasterEndurance(endurance), isAlive(true), selectedByGaster("") {}

bool Gaster::canBeSelected(const std::string& selector, int currentCount) {
    if (selectedByGaster != "") return false;

    if (gasterRole == "Защитник") {
        int maxDefenders = (selector == "player") ? Config::Team::defendersCount : Config::Team::defendersCount;
        return currentCount < maxDefenders;
    } else {
        int maxShooters = (selector == "player") ? Config::Team::shootersCount : Config::Team::shootersCount;
        return currentCount < maxShooters;
    }
}
void Gaster::setEndurance(int newEndurance) {
    gasterEndurance = std::max(0, newEndurance);
    if (gasterEndurance <= 0) {
        isAlive = false;
    }
}

void Gaster::setAlive(bool alive) {
    isAlive = alive;
    if (!isAlive) {
        gasterEndurance = 0;
    }
}

void Gaster::setSelectedBy(const std::string& selector) {
    selectedByGaster = selector;
}

void Gaster::saveInitialStats(int attack, int accuracy, int defense, int reaction) {
    initialAttack = attack;
    initialAccuracy = accuracy;
    initialDefense = defense;
    initialReaction = reaction;

    updateDerivedStats();
}

void Gaster::resetTacticEffects() {
    currentAttackMultiplier = 1.0f;
    currentDefenseMultiplier = 1.0f;
    currentFatigueMultiplier = 1.0f;

    currentAttack = initialAttack;
    currentAccuracy = initialAccuracy;
    currentDefense = initialDefense;
    currentReaction = initialReaction;

    updateDerivedStats();
}

void Gaster::applyTacticEffects(float attackMult, float defenseMult, float fatigueMult) {
    currentAttackMultiplier = attackMult;
    currentDefenseMultiplier = defenseMult;
    currentFatigueMultiplier = fatigueMult;
    updateDerivedStats();
}

void Gaster::clearProtectedShooters() {
    protectedShooterIndicesList.clear();
}

void Gaster::addProtectedShooter(int shooterIndex) {
    protectedShooterIndicesList.push_back(shooterIndex);
}

std::string Gaster::name() const { return gasterName; }
std::string Gaster::role() const { return gasterRole; }
int Gaster::endurance() const { return gasterEndurance; }
bool Gaster::alive() const { return isAlive; }
int Gaster::getOriginalAttack() const { return initialAttack; }
int Gaster::getOriginalAccuracy() const { return initialAccuracy; }
int Gaster::getOriginalDefense() const { return initialDefense; }
int Gaster::getOriginalReaction() const { return initialReaction; }
float Gaster::getFatigueMultiplier() const { return currentFatigueMultiplier; }
const std::vector<int>& Gaster::protectedShooterIndices() const { return protectedShooterIndicesList; }
std::string Gaster::selectedBy() const { return selectedByGaster; }
int Gaster::protectedShootersCount() const { return protectedShooterIndicesList.size(); }

int Gaster::getAttack() const { return currentAttack; }
int Gaster::getAccuracy() const { return currentAccuracy; }
int Gaster::getDefense() const { return currentDefense; }
int Gaster::getReaction() const { return currentReaction; }

int Gaster::getPrimaryStat() const { return 0; }
int Gaster::getSecondaryStat() const { return 0; }
