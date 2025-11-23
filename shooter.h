#ifndef SHOOTER_H
#define SHOOTER_H

#include "gaster.h"

class Shooter : public Gaster {
private:
    int attackPower;
    int accuracyValue;

public:
    Shooter(const std::string& name, int endurance, int attack, int accuracy);

    int getAttack() override;
    int getAccuracy() override;
    int getPrimaryStat() override;
    int getSecondaryStat() override;
    std::string getType() override;

    void setAttack(int attack) override;
    void setAccuracy(int accuracy) override;

    Element renderCard(bool isSelected, bool isFocused, bool isAttacker = false, bool showProtection = false, int protectedBy = -1) override;
};

#endif // SHOOTER_H
