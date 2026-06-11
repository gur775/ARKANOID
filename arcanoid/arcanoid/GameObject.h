#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
protected:
    sf::RectangleShape shape;

public:
    GameObject(sf::Vector2f position, sf::Vector2f size) {
        shape.setPosition(position);
        shape.setSize(size);
    }
    virtual ~GameObject() = default;

    virtual void draw(sf::RenderWindow& window) = 0;
    const sf::RectangleShape& getShape() const { return shape; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
};
