#include "BlockFactory.h"
#include "BlockTypes.h"
#include <cstdlib>

std::unique_ptr<Block> BlockFactory::createRandomBlock(sf::Vector2f position, sf::Vector2f size) {
    int randType = std::rand() % 10;

    switch (randType) {
    case 0:
        return std::make_unique<IndestructibleBlock>(position, size);
    case 1:
    case 2:
        return std::make_unique<BonusBlock>(position, size);
    case 3:
        return std::make_unique<SpeedUpBlock>(position, size);
    default:
        return std::make_unique<NormalBlock>(position, size);
    }
}