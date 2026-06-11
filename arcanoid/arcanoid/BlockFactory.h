#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "Block.h"

class BlockFactory {
public:
    static std::unique_ptr<Block> createRandomBlock(sf::Vector2f position, sf::Vector2f size);
};
