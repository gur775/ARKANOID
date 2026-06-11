#pragma once
#include <SFML/Graphics.hpp>

class Carriage {
private:
    sf::RectangleShape shape;
    float speed = 500.f;
    float baseWidth = 120.f;

public:
    Carriage(sf::Vector2f position) {
        shape.setSize({ baseWidth, 20.f });
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin({ baseWidth / 2.f, 10.f });
        shape.setPosition(position);
    }
    void setWidth(float width) { shape.setSize({ width, 20.f }); shape.setOrigin({ width / 2.f, 10.f }); }
    void moveLeft(float dt, int windowWidth) { if (shape.getPosition().x - shape.getSize().x / 2.f > 0) shape.move({ -speed * dt, 0.f }); }
    void moveRight(float dt, int windowWidth) { if (shape.getPosition().x + shape.getSize().x / 2.f < windowWidth) shape.move({ speed * dt, 0.f }); }
    void draw(sf::RenderWindow& window) { window.draw(shape); }
    const sf::RectangleShape& getShape() const { return shape; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
};
