#ifndef DEFENDER_H
#define DEFENDER_H

#include "gaster.h"

class Defender : public Gaster {
private:
    int defensePower;
    int reactionValue;

public:
    Defender(const std::string& name, int endurance, int defense, int reaction);

    int getDefense() const override;
    int getReaction() const override;
    int getPrimaryStat() const override;
    int getSecondaryStat() const override;
    std::string getType() const override;

    void setDefenseWithUpdate(int value) override;
    void setDefense(int defender) override;
    void setReaction(int reaction) override;

    Element renderCard(bool isSelected, bool isFocused, bool isAttacker = false,
                       bool showProtection = false, int protectedBy = -1,
                       const std::string& currentTactic = "",
                       bool isTacticPhase = true) override;

    void resetTacticEffects() override;
    void updateDerivedStats() override;
    bool canBeSelected(const std::string& selector, int currentCount) override;
};

#endif // DEFENDER_H
