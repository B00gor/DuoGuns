#ifndef DEFENDER_H
#define DEFENDER_H

#include "gaster.h"

class Defender : public Gaster {
private:
    int defensePower;
    int reactionValue;

public:
    Defender(const std::string& name, int endurance, int defense, int reaction);

    int getDefense() override;
    int getReaction() override;
    int getPrimaryStat() override;
    int getSecondaryStat() override;
    std::string getType() override;

    void setDefense(int defense) override;
    void setReaction(int reaction) override;

    Element renderCard(bool isSelected, bool isFocused, bool isAttacker = false, bool showProtection = false, int protectedBy = -1) override;
};

#endif // DEFENDER_H
