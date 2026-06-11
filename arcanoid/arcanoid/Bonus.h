#pragma once
#include "GameObject.h"

class Game;

class Bonus : public GameObject {
protected:
    bool active = true;

public:
    Bonus(sf::Vector2f position, sf::Color color) : GameObject(position, { 15.f, 15.f }) {
        shape.setOrigin({ 7.5f, 7.5f });
        shape.setFillColor(color);
    }
    virtual ~Bonus() = default;

    void update(float dt) { shape.move({ 0.f, 150.f * dt }); }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }

    virtual void applyEffect(Game& game) = 0;

    bool isActive() const { return active; }
    void deactivate() { active = false; }
};
