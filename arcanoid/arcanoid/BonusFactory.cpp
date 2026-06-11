#include "BonusFactory.h"
#include "BonusTypes.h"
#include <cstdlib>

std::unique_ptr<Bonus> BonusFactory::createRandomBonus(sf::Vector2f position) {
    int randType = std::rand() % 4;

    switch (randType) {
    case 0:  return std::make_unique<FloorBonus>(position);
    case 1:  return std::make_unique<ShrinkBonus>(position);
    case 2:  return std::make_unique<StickyBonus>(position);
    default: return std::make_unique<BallBonus>(position);
    }
}