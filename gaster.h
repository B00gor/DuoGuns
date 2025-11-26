#pragma once
#include <string>
#include <vector>
#include "config.h"

class Gaster {
protected:
    std::string gasterName;
    std::string gasterRole;
    int gasterEndurance;
    bool isAlive;
    std::string selectedByGaster;

    int initialAttack = 0;
    int initialAccuracy = 0;
    int initialDefense = 0;
    int initialReaction = 0;

    int currentAttack = 0;
    int currentAccuracy = 0;
    int currentDefense = 0;
    int currentReaction = 0;

    float currentAttackMultiplier = 1.0f;
    float currentDefenseMultiplier = 1.0f;
    float currentFatigueMultiplier = 1.0f;

    std::vector<int> protectedShooterIndicesList;

public:
    Gaster(const std::string& name, const std::string& role, int endurance);

    std::string name() const;
    std::string role() const;
    int endurance() const;
    bool alive() const;

    int getOriginalAttack() const;
    int getOriginalAccuracy() const;
    int getOriginalDefense() const;
    int getOriginalReaction() const;
    float getFatigueMultiplier() const;

    virtual int getAttack() const;
    virtual int getAccuracy() const;
    virtual int getDefense() const;
    virtual int getReaction() const;
    virtual std::string getType() const = 0;

    virtual int getPrimaryStat() const;
    virtual int getSecondaryStat() const;

    virtual void setEndurance(int newEndurance);
    virtual void setAlive(bool alive);
    virtual void setSelectedBy(const std::string& selector);

    virtual void setAttackWithUpdate(int value) {}
    virtual void setDefenseWithUpdate(int value) {}
    virtual void setAttack(int value) {}
    virtual void setAccuracy(int value) {}

    virtual void setDefense(int value) {}
    virtual void setReaction(int value) {}

    virtual void saveInitialStats(int attack, int accuracy, int defense, int reaction);
    virtual void resetTacticEffects();
    virtual void updateDerivedStats() {}
    virtual void applyTacticEffects(float attackMult, float defenseMult, float fatigueMult);

    virtual void clearProtectedShooters();
    virtual void addProtectedShooter(int shooterIndex);

    const std::vector<int>& protectedShooterIndices() const;
    std::string selectedBy() const;
    int protectedShootersCount() const;

    virtual Element renderCard(bool isSelected, bool isFocused, bool isAttacker = false,
                       bool showProtection = false, int protectedBy = -1,
                       const std::string& currentTactic = "",
                       bool isTacticPhase = true) = 0;

    virtual bool canBeSelected(const std::string& selector, int currentCount) = 0;
};
