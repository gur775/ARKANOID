#pragma once
#include "Bonus.h"

class FloorBonus : public Bonus {
public:
    FloorBonus(sf::Vector2f position) : Bonus(position, sf::Color::Magenta) {}
    void applyEffect(Game& game) override;
};

class ShrinkBonus : public Bonus {
public:
    ShrinkBonus(sf::Vector2f position) : Bonus(position, sf::Color::Red) {}
    void applyEffect(Game& game) override;
};

class StickyBonus : public Bonus {
public:
    StickyBonus(sf::Vector2f position) : Bonus(position, sf::Color::Blue) {}
    void applyEffect(Game& game) override;
};

class SecondBallBonus : public Bonus {
public:
    SecondBallBonus(sf::Vector2f position) : Bonus(position, sf::Color::White) {}
    void applyEffect(Game& game) override;
};
