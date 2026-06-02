#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
// Размеры экрана
const int Width = 800;
const int Height = 600;

// Типы блоков
enum class BlockType {
    Normal,         // Обычный
    Indestructible, // Неразрушаемый
    WithBonus,      // с бонусом
    Booster         // Ускоряющий мяч
};
// Виды бонусов
enum class BonusType {
    Floor,    //Одноразовое дно
    ShrinkCarriage,   //Уменьшение каретки на 15 секунд
    StickyCarriage,   //Прилипание
    SecondBall      //Второй мяч
};

//Структура бонуса
struct Bonus {
    sf::RectangleShape shape;
    BonusType type;
    bool active = true;

    Bonus(sf::Vector2f position, BonusType t) : type(t) {
        shape.setSize({ 15.f, 15.f });
        shape.setOrigin({ 7.5f, 7.5f });
        shape.setPosition(position);

        // Цвета бонусов
        switch (type) {
        case BonusType::Floor:  shape.setFillColor(sf::Color::Magenta); break; // Розовый - Дно
        case BonusType::ShrinkCarriage: shape.setFillColor(sf::Color::Red); break;     // Красный - Уменьшение
        case BonusType::StickyCarriage: shape.setFillColor(sf::Color::Blue); break;    // Синий - прилипание
        case BonusType::SecondBall:   shape.setFillColor(sf::Color::White); break;   // Белый - 2-й шарик
        }
    }

    void update(float dt) {
        shape.move({ 0.f, 150.f * dt });
    }
};

// Блок
struct Block {
    sf::RectangleShape shape;
    BlockType type;
    int health;
    bool destroyed = false;

    Block(sf::Vector2f position, sf::Vector2f size, BlockType t) : type(t) {
        shape.setPosition(position);
        shape.setSize(size);

        if (type == BlockType::Indestructible) {
            health = -1;
            shape.setFillColor(sf::Color::White);
        }
        else {
            health = std::rand() % 3 + 1;
            updateColor();
        }

        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
    }
    //Блок темнеет при попадании. Количество его очков уменьшается
    void updateColor() {
        if (type == BlockType::Indestructible) return;

        if (type == BlockType::Booster) {
            shape.setFillColor(sf::Color::Cyan);
        }
        else if (type == BlockType::WithBonus) {
            shape.setFillColor(sf::Color::Yellow);
        }
        else {
            if (health == 3)      shape.setFillColor(sf::Color(255, 0, 0));
            else if (health == 2) shape.setFillColor(sf::Color(180, 50, 50));
            else                  shape.setFillColor(sf::Color(110, 50, 50));
        }
    }

    void hit() {
        if (type != BlockType::Indestructible) {
            health--;
            if (health <= 0) destroyed = true;
            else updateColor();
        }
    }
};
//Мяч
struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool isAlive = true;
    bool isStuck = false;      
    float stickyOffsetX = 0.f;
    const float nullSpeed = 300.f;

    Ball(sf::Vector2f position) {
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color::Green);
        shape.setOrigin({ 10.f, 10.f });
        reset(position);
    }

    void reset(sf::Vector2f position) {
        shape.setPosition(position);
        velocity = { 150.f, -nullSpeed };
        isAlive = true;
        isStuck = false;
    }

    void update(float dt, sf::Vector2f carriagePos) {
        if (!isAlive) return;

        if (isStuck) {
            shape.setPosition({ carriagePos.x + stickyOffsetX, carriagePos.y - 20.f });
        }
        else {
            shape.move(velocity * dt);
        }
    }
};

//Каретка
struct Carriage {
    sf::RectangleShape shape;
    float speed = 500.f;
    float baseWidth = 120.f;

    Carriage(sf::Vector2f position) {
        shape.setSize({ baseWidth, 20.f });
        shape.setFillColor(sf::Color::Blue);
        shape.setOrigin({ baseWidth / 2.f, 10.f });
        shape.setPosition(position);
    }

    void setWidth(float width) {
        shape.setSize({ width, 20.f });
        shape.setOrigin({ width / 2.f, 10.f });
    }

    void moveLeft(float dt) {
        if (shape.getPosition().x - shape.getSize().x / 2.f > 0) {
            shape.move({ -speed * dt, 0.f });
        }
    }

    void moveRight(float dt) {
        if (shape.getPosition().x + shape.getSize().x / 2.f < Width) {
            shape.move({ speed * dt, 0.f });
        }
    }
};

// Проверка коллизий
bool Collision(Ball& ball, const sf::RectangleShape& rect, bool isBlock, Block* block = nullptr, bool isCarriage = false, bool stickyActive = false) {
    if (!ball.isAlive || ball.isStuck) return false;

    sf::FloatRect ballBounds = ball.shape.getGlobalBounds();
    sf::FloatRect rectBounds = rect.getGlobalBounds();

    auto intersectionOpt = ballBounds.findIntersection(rectBounds);
    if (!intersectionOpt) return false;

    sf::FloatRect overlap = *intersectionOpt;

    // Если это каретка и активирован бонус прилипания
    if (isCarriage && stickyActive) {
        ball.isStuck = true;
        ball.stickyOffsetX = ball.shape.getPosition().x - rect.getPosition().x;
        return true;
    }

    // Отскок
    if (overlap.size.x < overlap.size.y) {
        if (ballBounds.position.x < rectBounds.position.x) {
            ball.velocity.x = -std::abs(ball.velocity.x);
        }
        else {
            ball.velocity.x = std::abs(ball.velocity.x);
        }
    }
    else {
        if (ballBounds.position.y < rectBounds.position.y) {
            ball.velocity.y = -std::abs(ball.velocity.y);
        }
        else {
            ball.velocity.y = std::abs(ball.velocity.y);
        }
    }

    if (isBlock && block) {
        if (block->type == BlockType::Booster) {
            ball.velocity *= 1.15f;
        }
        block->hit();
    }

    return true;
}

// Перезапуск игры в случае победы или поражения
void resetGame(Carriage& carriage, std::vector<Ball>& balls, std::vector<Block>& blocks, std::vector<Bonus>& bonuses,
    int& Lives, bool& isFloor, bool& isSticky, bool& isDecline,
    float& declineTimer, bool& gameOver, bool& gameWon) {
    Lives = 3;
    isFloor = false;
    isSticky = false;
    isDecline = false;
    declineTimer = 0.f;
    gameOver = false;
    gameWon = false;

    carriage.shape.setPosition({ Width / 2.f, Height - 40.f });
    carriage.setWidth(120.f);

    balls.clear();
    balls.push_back(Ball({ Width / 2.f, Height - 100.f }));

    bonuses.clear();
    blocks.clear();

    int rows = 5;
    int cols = 10;
    sf::Vector2f blockSize(70.f, 25.f);
    float startX = 45.f;
    float startY = 60.f;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::Vector2f pos(startX + j * (blockSize.x + 5), startY + i * (blockSize.y + 5));
            int randType = std::rand() % 10;
            BlockType type = BlockType::Normal;

            if (randType == 0) type = BlockType::Indestructible;
            else if (randType == 1) type = BlockType::WithBonus;
            else if (randType == 2) type = BlockType::Booster;

            blocks.push_back(Block(pos, blockSize, type));
        }
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Создание игрового окна
    sf::RenderWindow window(sf::VideoMode({Width, Height}), "Arkanoid SFML 3");
    window.setFramerateLimit(60);

    // Загрузка шрифта
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
    }
    // Текстовые надписи на блоках и экране
    sf::Text livesText(font);
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition({ Width - 120.f, 15.f });

    sf::Text blockHealthText(font);
    blockHealthText.setCharacterSize(14);
    blockHealthText.setFillColor(sf::Color::Black);

    sf::Text stateText(font);
    stateText.setCharacterSize(36);
    stateText.setFillColor(sf::Color::White);

    Carriage carriage({ Width / 2.f, Height - 40.f });
    std::vector<Ball> balls;
    std::vector<Block> blocks;
    std::vector<Bonus> bonuses;
    //Флаги
    int Lives = 3; //Количество жизней
    bool isSticky = false; //Бонус с прилипанием
    bool isDecline = false; //Бонус с уменьшением каретки
    float declineTimer = 0.f; //Время действия бонуса с уменьшением каретки 
    bool isFloor = false; //Одноразовое дно
    bool gameOver = false; //Конец игры
    bool gameWon = false; //Победа в игре

    sf::RectangleShape Floor({ static_cast<float>(Width), 10.f });
    Floor.setPosition({ 0.f, Height - 10.f });
    Floor.setFillColor(sf::Color::Yellow);

    sf::RectangleShape overlay({ static_cast<float>(Width), static_cast<float>(Height) });
    overlay.setPosition({ 0.f, 0.f });

    // Первый запуск
    resetGame(carriage, balls, blocks, bonuses, Lives, isFloor, isSticky, isDecline, declineTimer, gameOver, gameWon);

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>()) {
                // Перезапуск игры
                if ((gameOver || gameWon) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                    resetGame(carriage, balls, blocks, bonuses, Lives, isFloor, isSticky, isDecline, declineTimer, gameOver, gameWon);
                    clock.restart();
                }
                // Отпускание прилипшего шарика по нажатию пробела
                if (!gameOver && !gameWon && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                    for (auto& ball : balls) {
                        if (ball.isStuck) {
                            ball.isStuck = false;
                            isSticky = false;
                        }
                    }
                }
            }
        }

        float dt = clock.restart().asSeconds();

        if (!gameOver && !gameWon) {
            // Таймер бонуса с уменьшением каретки
            if (isDecline) {
                declineTimer -= dt;
                if (declineTimer <= 0.f) {
                    isDecline = false;
                    carriage.setWidth(120.f);
                }
            }

            // Управление кареткой
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                carriage.moveLeft(dt);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                carriage.moveRight(dt);
            }

            // Обновление шариков
            int activeBallsCount = 0;
            for (auto& ball : balls) {
                if (ball.isAlive) {
                    ball.update(dt, carriage.shape.getPosition());
                    activeBallsCount++;

                    // Отскоки от стен
                    if (ball.shape.getPosition().x - ball.shape.getRadius() < 0) {
                        ball.shape.setPosition({ ball.shape.getRadius(), ball.shape.getPosition().y });
                        ball.velocity.x = std::abs(ball.velocity.x);
                    }
                    if (ball.shape.getPosition().x + ball.shape.getRadius() > Width) {
                        ball.shape.setPosition({ Width - ball.shape.getRadius(), ball.shape.getPosition().y });
                        ball.velocity.x = -std::abs(ball.velocity.x);
                    }
                    if (ball.shape.getPosition().y - ball.shape.getRadius() < 0) {
                        ball.shape.setPosition({ ball.shape.getPosition().x, ball.shape.getRadius() });
                        ball.velocity.y = std::abs(ball.velocity.y);
                    }

                    // Если одноразовое дно
                    if (isFloor) {
                        if (Collision(ball, Floor, false)) {
                            isFloor = false;
                        }
                    }
                    // Шарик упал
                    else if (ball.shape.getPosition().y > Height) {
                        ball.isAlive = false;
                    }

                    // Коллизия с кареткой
                    Collision(ball, carriage.shape, false, nullptr, true, isSticky);
                }
            }

            // Проверка: если на поле нет мячей
            if (activeBallsCount == 0) {
                Lives--;
                if (Lives <= 0) {
                    gameOver = true;
                }
                else {
                    // Возвращаем один шарик на поле
                    balls.clear();
                    balls.push_back(Ball({ Width / 2.f, Height - 100.f }));
                }
            }

            // Коллизии с блоками
            bool anyDestructibleLeft = false;
            for (auto& block : blocks) {
                if (!block.destroyed) {
                    if (block.type != BlockType::Indestructible) {
                        anyDestructibleLeft = true;
                    }

                    for (auto& ball : balls) {
                        if (Collision(ball, block.shape, true, &block)) {
                            if (block.type != BlockType::Indestructible) {
                                if (Lives < 3) Lives++;

                                // Вызов случайного бонуса при уничтожении блока с бонусом
                                if (block.destroyed && block.type == BlockType::WithBonus) {
                                    int randBonus = std::rand() % 4;
                                    BonusType bType = static_cast<BonusType>(randBonus);
                                    bonuses.push_back(Bonus(block.shape.getPosition() + sf::Vector2f(35.f, 12.f), bType));
                                }
                            }
                        }
                    }
                }
            }

            if (!anyDestructibleLeft) gameWon = true;

            // Падающие бонусы
            for (auto& bonus : bonuses) {
                if (bonus.active) {
                    bonus.update(dt);

                    // Поимка бонуса кареткой
                    if (bonus.shape.getGlobalBounds().findIntersection(carriage.shape.getGlobalBounds()).has_value()) {
                        bonus.active = false;

                        if (bonus.type == BonusType::Floor) {
                            isFloor = true; //Активируем одноразовое дно
                        }
                        else if (bonus.type == BonusType::ShrinkCarriage) {
                            isDecline = true; //Уменьшение каретки на 15 секунд
                            declineTimer = 15.f;
                            carriage.setWidth(70.f);
                        }
                        else if (bonus.type == BonusType::StickyCarriage) {
                            isSticky = true; // Активирует прилипание
                        }
                        else if (bonus.type == BonusType::SecondBall) {
                            // Появляется второй мяч из каретки
                            balls.push_back(Ball({ carriage.shape.getPosition().x, carriage.shape.getPosition().y - 30.f }));
                        }
                    }
                    if (bonus.shape.getPosition().y > Height) bonus.active = false;
                }
            }
            bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), [](const Bonus& b) { return !b.active; }), bonuses.end());
        }

        //Изображения
        window.clear(sf::Color::Black);

        // Рисуем элементы, если игра активна
        window.draw(carriage.shape);

        for (const auto& ball : balls) {
            if (ball.isAlive) window.draw(ball.shape);
        }

        for (const auto& block : blocks) {
            if (!block.destroyed) {
                window.draw(block.shape);
                if (block.type != BlockType::Indestructible) {
                    blockHealthText.setString(std::to_string(block.health));
                    sf::Vector2f blockPos = block.shape.getPosition();
                    blockHealthText.setPosition({ blockPos.x + 30.f, blockPos.y + 2.f });
                    window.draw(blockHealthText);
                }
            }
        }

        for (const auto& bonus : bonuses) {
            window.draw(bonus.shape);
        }

        if (isFloor) window.draw(Floor);

        livesText.setString("Lives: " + std::to_string(Lives));
        window.draw(livesText);

        if (gameOver) {
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);
            stateText.setString("GAME OVER\nPress Space to Restart");
            stateText.setPosition({ Width / 2.f - 180.f, Height / 2.f - 50.f });
            window.draw(stateText);
        }
        else if (gameWon) {
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);
            stateText.setString("VICTORY!\nPress Space to Restart");
            stateText.setPosition({ Width / 2.f - 160.f, Height / 2.f - 50.f });
            window.draw(stateText);
        }

        window.display();
    }

    return 0;
}