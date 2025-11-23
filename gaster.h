#ifndef GASTER_H
#define GASTER_H

#include <string>
#include <vector>
#include <ftxui/dom/elements.hpp>
#include "config.h"

using namespace ftxui;

class Gaster {
protected:
    std::string gasterName;
    std::string gasterRole;
    int gasterEndurance;
    bool isAlive;
    std::string selectedByGaster;
    std::vector<int> protectedShooterIndicesList;

public:
    Gaster(const std::string& name, const std::string& role, int endurance);
    virtual ~Gaster() = default;

    std::string& name();
    std::string& role();
    int endurance();
    bool alive();
    std::string& selectedBy();
    std::vector<int>& protectedShooterIndices();
    int protectedShootersCount();

    void setSelectedBy(const std::string& selector);
    void setEndurance(int endurance);
    void setAlive(bool alive);
    void addProtectedShooter(int shooterIndex);
    void clearProtectedShooters();

    virtual void setAttack(int attack) {}
    virtual void setAccuracy(int accuracy) {}
    virtual void setDefense(int defense) {}
    virtual void setReaction(int reaction) {}

    virtual int getAttack();
    virtual int getAccuracy();
    virtual int getDefense();
    virtual int getReaction();

    virtual std::string getType() = 0;
    virtual int getPrimaryStat() = 0;
    virtual int getSecondaryStat() = 0;
    virtual Element renderCard(bool isSelected, bool isFocused, bool isAttacker = false, bool showProtection = false, int protectedBy = -1) = 0;
    bool canBeSelected(const std::string& selector, int currentCount);
};

#endif // GASTER_H
