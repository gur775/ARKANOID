#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Carriage.h"
#include "Ball.h"
#include "Block.h"
#include "Bonus.h"

class Game {
private:
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    sf::RenderWindow window;
    sf::Font font;
    std::unique_ptr<sf::Text> livesText;
    std::unique_ptr<sf::Text> blockHealthText;
    std::unique_ptr<sf::Text> stateText;

    Carriage carriage;
    std::vector<Ball> balls;
    std::vector<std::unique_ptr<Block>> blocks;
    std::vector<std::unique_ptr<Bonus>> bonuses;

    int playerLives = 3;
    bool safetyFloorActive = false;
    bool stickyActive = false;
    bool shrunkActive = false;
    float shrinkTimer = 0.f;
    bool gameOver = false;
    bool gameWon = false;

    sf::RectangleShape safetyFloor;
    sf::RectangleShape overlay;

    void initUI();
    void reset();
    void processEvents();
    void update(float dt);
    void render();
    bool checkCollision(Ball& ball, const sf::RectangleShape& rect, bool isBlock, Block* block = nullptr, bool isCarriage = false);

public:
    Game();
    void run();

    void activateSafetyFloor() { safetyFloorActive = true; }
    void activateStickyCarriage() { stickyActive = true; }
    void activateShrinkCarriage();
    void spawnSecondBall();
};