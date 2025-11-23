#include "gaster.h"

Gaster::Gaster(const std::string& name, const std::string& role, int Endurance)
    : gasterName(name), gasterRole(role), gasterEndurance(Endurance), isAlive(true), selectedByGaster("") {}

std::string& Gaster::name() { return gasterName; }
std::string& Gaster::role() { return gasterRole; }
int Gaster::endurance() { return gasterEndurance; }
bool Gaster::alive() { return isAlive; }
std::string& Gaster::selectedBy() { return selectedByGaster; }
std::vector<int>& Gaster::protectedShooterIndices() { return protectedShooterIndicesList; }
int Gaster::protectedShootersCount() { return protectedShooterIndicesList.size(); }

void Gaster::setSelectedBy(const std::string& selector) { selectedByGaster = selector; }
void Gaster::setEndurance(int Endurance) { gasterEndurance = Endurance; }
void Gaster::setAlive(bool alive) { isAlive = alive; }
void Gaster::addProtectedShooter(int shooterIndex) { protectedShooterIndicesList.push_back(shooterIndex); }
void Gaster::clearProtectedShooters() { protectedShooterIndicesList.clear(); }

int Gaster::getAttack() { return 0; }
int Gaster::getAccuracy() { return 0; }
int Gaster::getDefense() { return 0; }
int Gaster::getReaction() { return 0; }

bool Gaster::canBeSelected(const std::string& selector, int currentCount) {
    if (selectedByGaster != "") return false;

    if (gasterRole == "Защитник") {
        int maxDefenders = (selector == "Gaster") ? Config::Team::defendersCount : Config::Team::defendersCount;
        return currentCount < maxDefenders;
    } else {
        int maxShooters = (selector == "Gaster") ? Config::Team::shootersCount : Config::Team::shootersCount;
        return currentCount < maxShooters;
    }
}
