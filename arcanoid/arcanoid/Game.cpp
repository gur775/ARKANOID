#include "Game.h"
#include "BlockFactory.h" 
#include "BonusFactory.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game() :
    window(sf::VideoMode({ static_cast<unsigned>(WINDOW_WIDTH), static_cast<unsigned>(WINDOW_HEIGHT) }), "Arkanoid OOP with Factories"),
    carriage({ 400.f, 560.f }),
    safetyFloor({ 800.f, 10.f }),
    overlay({ 800.f, 600.f }),
    
    livesText(nullptr),
    blockHealthText(nullptr),
    stateText(nullptr)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    safetyFloor.setPosition({ 0.f, WINDOW_HEIGHT - 10.f });
    safetyFloor.setFillColor(sf::Color::Yellow);
    overlay.setPosition({ 0.f, 0.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 200));

    initUI();
    reset();
}

void Game::initUI() {
    
    if (!font.openFromFile("arial.ttf")) {
        return;
    }

    livesText = std::make_unique<sf::Text>(font, "Lives: 3", 20);
    livesText->setFillColor(sf::Color::White);
    livesText->setPosition({ WINDOW_WIDTH - 120.f, 15.f });

    blockHealthText = std::make_unique<sf::Text>(font, "", 14);
    blockHealthText->setFillColor(sf::Color::Black);

    stateText = std::make_unique<sf::Text>(font, "", 36);
    stateText->setFillColor(sf::Color::White);
}

void Game::reset() {
    playerLives = 3;
    safetyFloorActive = false;
    stickyActive = false;
    shrunkActive = false;
    shrinkTimer = 0.f;
    gameOver = false;
    gameWon = false;

    carriage.setWidth(120.f);
    balls.clear();
    balls.push_back(Ball({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f }));
    bonuses.clear();
    blocks.clear();

    sf::Vector2f blockSize(70.f, 25.f);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 10; ++j) {
            sf::Vector2f pos(45.f + j * 75.f, 60.f + i * 30.f);

            blocks.push_back(BlockFactory::createRandomBlock(pos, blockSize));
        }
    }
}

void Game::activateShrinkCarriage() { shrunkActive = true; shrinkTimer = 10.f; carriage.setWidth(70.f); }
void Game::spawnSecondBall() { balls.push_back(Ball({ carriage.getPosition().x, carriage.getPosition().y - 30.f })); }

bool Game::checkCollision(Ball& ball, const sf::RectangleShape& rect, bool isBlock, Block* block, bool isCarriage) {
    if (!ball.isAlive() || ball.isStuck()) return false;
    auto intersectOpt = ball.getShape().getGlobalBounds().findIntersection(rect.getGlobalBounds());
    if (!intersectOpt) return false;

    if (isCarriage && stickyActive) {
        ball.setStuck(true);
        ball.setStickyOffsetX(ball.getShape().getPosition().x - rect.getPosition().x);
        return true;
    }

    sf::FloatRect overlap = *intersectOpt;
    sf::Vector2f vel = ball.getVelocity();
    if (overlap.size.x < overlap.size.y) vel.x = (ball.getShape().getGlobalBounds().position.x < rect.getGlobalBounds().position.x) ? -std::abs(vel.x) : std::abs(vel.x);
    else vel.y = (ball.getShape().getGlobalBounds().position.y < rect.getGlobalBounds().position.y) ? -std::abs(vel.y) : std::abs(vel.y);
    ball.setVelocity(vel);

    if (isBlock && block) {
        block->onHit();            
        block->applyBallEffect(ball); 
    }
    return true;
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
        if (event->is<sf::Event::KeyPressed>()) {
            if ((gameOver || gameWon) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) reset();
            if (!gameOver && !gameWon && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                for (auto& ball : balls) { if (ball.isStuck()) { ball.setStuck(false); stickyActive = false; } }
            }
        }
    }
}

void Game::update(float dt) {
    if (gameOver || gameWon) return;

    if (shrunkActive) { shrinkTimer -= dt; if (shrinkTimer <= 0.f) { shrunkActive = false; carriage.setWidth(120.f); } }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) carriage.moveLeft(dt, WINDOW_WIDTH);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) carriage.moveRight(dt, WINDOW_WIDTH);

    int activeBalls = 0;
    for (auto& ball : balls) {
        if (!ball.isAlive()) continue;
        ball.update(dt, carriage.getPosition());
        activeBalls++;

        sf::Vector2f p = ball.getShape().getPosition();
        sf::Vector2f v = ball.getVelocity();
        float r = ball.getShape().getRadius();

        if (p.x - r < 0) { ball.getShape().setPosition({ r, p.y }); v.x = std::abs(v.x); ball.setVelocity(v); }
        if (p.x + r > WINDOW_WIDTH) { ball.getShape().setPosition({ WINDOW_WIDTH - r, p.y }); v.x = -std::abs(v.x); ball.setVelocity(v); }
        if (p.y - r < 0) { ball.getShape().setPosition({ p.x, r }); v.y = std::abs(v.y); ball.setVelocity(v); }

        if (safetyFloorActive && checkCollision(ball, safetyFloor, false)) safetyFloorActive = false;
        else if (p.y > WINDOW_HEIGHT) ball.setAlive(false);

        checkCollision(ball, carriage.getShape(), false, nullptr, true);
    }

    if (activeBalls == 0) {
        playerLives--;
        if (playerLives <= 0) gameOver = true;
        else { balls.clear(); balls.push_back(Ball({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 100.f })); }
    }

    bool anyDestructible = false;
    for (auto& block : blocks) {
        if (!block->isDestroyed()) {

            
            if (block->isWinCondition()) {
                anyDestructible = true;
            }

            for (auto& ball : balls) {
                
                if (checkCollision(ball, block->getShape(), true, block.get())) {

                    
                    if (block->isDestroyed() && block->shouldSpawnBonus()) {
                        sf::Vector2f spawnPos = block->getPosition() + sf::Vector2f(35.f, 12.f);
                        bonuses.push_back(BonusFactory::createRandomBonus(spawnPos));
                    }
                }
            }
        }
    }
    if (!anyDestructible) gameWon = true;

    for (auto& bonus : bonuses) {
        if (bonus->isActive()) {
            bonus->update(dt);
            if (bonus->getShape().getGlobalBounds().findIntersection(carriage.getShape().getGlobalBounds())) {
                bonus->deactivate();
                bonus->applyEffect(*this);
            }
            if (bonus->getPosition().y > WINDOW_HEIGHT) bonus->deactivate();
        }
    }
    bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), [](const auto& b) { return !b->isActive(); }), bonuses.end());
}

void Game::render() {
    window.clear(sf::Color::Black);
    carriage.draw(window);
    for (auto& ball : balls) ball.draw(window);
    for (auto& block : blocks) {
        if (!block->isDestroyed()) {
            block->draw(window);
            
            if (block->isWinCondition() && blockHealthText) {
                blockHealthText->setString(std::to_string(block->getHealth()));
                blockHealthText->setPosition(block->getPosition() + sf::Vector2f(30.f, 2.f));
                window.draw(*blockHealthText); 
            }
        }
    }
    for (auto& bonus : bonuses) bonus->draw(window);
    if (safetyFloorActive) window.draw(safetyFloor);

    if (livesText) {
        livesText->setString("Lives: " + std::to_string(playerLives));
        window.draw(*livesText);
    }

    if (gameOver || gameWon && stateText) {
        window.draw(overlay);
        stateText->setString(gameOver ? "GAME OVER\nPress SPACE to Restart" : "VICTORY!\nPress SPACE to Restart");
        sf::FloatRect textBounds = stateText->getGlobalBounds();
        stateText->setPosition({ WINDOW_WIDTH / 2.f - textBounds.size.x / 2.f, WINDOW_HEIGHT / 2.f - 50.f });
        window.draw(*stateText);
    }

    window.display();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        processEvents();
        update(clock.restart().asSeconds());
        render();
    }
}