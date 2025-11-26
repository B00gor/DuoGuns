#ifndef SHOOTER_H
#define SHOOTER_H

#include "gaster.h"

class Shooter : public Gaster {
private:
    int attackPower;
    int accuracyValue;

public:
    Shooter(const std::string& name, int endurance, int attack, int accuracy);

    int getAttack() const override;
    int getAccuracy() const override;
    int getPrimaryStat() const override;
    int getSecondaryStat() const override;
    std::string getType() const override;

    void setAttackWithUpdate(int value) override;
    void setAttack(int attack) override;
    void setAccuracy(int accuracy) override;

    void resetTacticEffects() override;
    void updateDerivedStats() override;
    bool canBeSelected(const std::string& selector, int currentCount) override;

    Element renderCard(bool isSelected, bool isFocused, bool isAttacker = false,
                       bool showProtection = false, int protectedBy = -1,
                       const std::string& currentTactic = "",
                       bool isTacticPhase = true) override;
};

#endif // SHOOTER_H
