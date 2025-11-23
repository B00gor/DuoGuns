#ifndef GASTERGENERATOR_H
#define GASTERGENERATOR_H

#include <vector>
#include <memory>
#include <string>
#include "gaster.h"
#include "shooter.h"
#include "defender.h"

class GasterGenerator {
public:
    GasterGenerator() = default;
    std::unique_ptr<Gaster> createShooter(std::string& name);
    std::unique_ptr<Gaster> createDefender(std::string& name);
    std::vector<std::unique_ptr<Gaster>> generateAllPlayers();
};

#endif // GASTERGENERATOR_H
