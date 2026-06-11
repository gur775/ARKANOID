#pragma once
#include "GameObject.h"
#include "Ball.h"
class Block : public GameObject {
protected:
    int health;
    bool destroyed = false;
    virtual void updateColor() = 0;

public:
    Block(sf::Vector2f position, sf::Vector2f size, int hp)
        : GameObject(position, size), health(hp) {
    }

    virtual ~Block() = default;

    virtual void onHit() {
        health--;
        if (health <= 0) destroyed = true;
        else updateColor();
    }
    virtual void applyBallEffect(Ball& ball) {
        
    }
    bool isDestroyed() const { return destroyed; }
    int getHealth() const { return health; }

    
    virtual bool isWinCondition() const { return true; }

    
    virtual bool shouldSpawnBonus() const { return false; }
};
