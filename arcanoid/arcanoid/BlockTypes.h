#pragma once
#include "Block.h"

class NormalBlock : public Block {
protected:
    void updateColor() override { 
        if (health == 3)      shape.setFillColor(sf::Color(255, 0, 0));
        else if (health == 2) shape.setFillColor(sf::Color(180, 50, 50));
        else                  shape.setFillColor(sf::Color(110, 50, 50));
    }
public:
    NormalBlock(sf::Vector2f position, sf::Vector2f size) : Block(position, size, std::rand() % 3 + 1) { updateColor(); }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};

class IndestructibleBlock : public Block {
protected:
    void updateColor() override {}

public:
    IndestructibleBlock(sf::Vector2f position, sf::Vector2f size) : Block(position, size, -1) {
        shape.setFillColor(sf::Color::White);
    }

    
    void onHit() override { }

    
    bool isWinCondition() const override { return false; }

    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};

class SpeedUpBlock : public Block {
protected:
    void updateColor() override { shape.setFillColor(sf::Color(0, 180, 180)); }

public:
    SpeedUpBlock(sf::Vector2f position, sf::Vector2f size) : Block(position, size, std::rand() % 3 + 1) {
        shape.setFillColor(sf::Color::Cyan);
    }

    
    void applyBallEffect(Ball& ball) override {
        ball.multiplyVelocity(1.15f); 
    }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};

class BonusBlock : public Block {
protected:
    void updateColor() override { shape.setFillColor(sf::Color(180, 180, 0)); }
public:
    BonusBlock(sf::Vector2f position, sf::Vector2f size) : Block(position, size, std::rand() % 3 + 1) {
        shape.setFillColor(sf::Color::Yellow);
    }
    
    bool shouldSpawnBonus() const override { return true; } 

    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};
