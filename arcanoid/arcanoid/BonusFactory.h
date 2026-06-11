#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Bonus.h"

class BonusFactory {
public:

    static std::unique_ptr<Bonus> createRandomBonus(sf::Vector2f position);
};
