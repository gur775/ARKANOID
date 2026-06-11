#pragma once
#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool alive = true;
    bool stuck = false;
    float stickyOffsetX = 0.f;
    const float initialSpeed = 300.f;

public:
    Ball(sf::Vector2f position) {
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color::Green);
        shape.setOrigin({ 10.f, 10.f });
        reset(position);
    }
    void reset(sf::Vector2f position) { shape.setPosition(position); velocity = { 150.f, -initialSpeed }; alive = true; stuck = false; }
    void update(float dt, sf::Vector2f carriagePos) {
        if (!alive) return;
        if (stuck) shape.setPosition({ carriagePos.x + stickyOffsetX, carriagePos.y - 20.f });
        else shape.move(velocity * dt);
    }
    void draw(sf::RenderWindow& window) { if (alive) window.draw(shape); }

    sf::CircleShape& getShape() { return shape; }
    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(sf::Vector2f vel) { velocity = vel; }
    void multiplyVelocity(float factor) { velocity *= factor; }
    bool isAlive() const { return alive; }
    void setAlive(bool state) { alive = state; }
    bool isStuck() const { return stuck; }
    void setStuck(bool state) { stuck = state; }
    void setStickyOffsetX(float offset) { stickyOffsetX = offset; }
};
