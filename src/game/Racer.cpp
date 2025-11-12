#include "Racer.h"
#include <cmath>
#include <iostream>

Racer::Racer() {
    // ✅ 创建资源
    itemTexture = std::make_shared<sf::Texture>();
    bgTexture   = std::make_shared<sf::Texture>();
    buffer      = std::make_shared<sf::SoundBuffer>();

    // ✅ 用纹理构建 Sprite
    itemSprite = std::make_shared<sf::Sprite>(*itemTexture);
    bgSprite   = std::make_shared<sf::Sprite>(*bgTexture);

    // ✅ 构建声音
    sound = std::make_shared<sf::Sound>(*buffer);
    bgm   = std::make_shared<sf::Sound>(*buffer);

    // ✅ 参数初始化
    cameraY = 1600;
    speed = 100;
    score = 0;
    isJumping = false;
    y = dy = 0;

    // ✅ 生成白贴图 (避免 SFML3 无默认构造错误)
    sf::Image img({450u * 5, 450u * 3}, sf::Color::White);
    itemTexture->loadFromImage(img);

    roads.reserve(100);
    for (int i = 0; i < 100; ++i)
        roads.emplace_back(0, 0, (i + 1) * 180, 0.5f, *itemSprite);
}

void Racer::handleInput() {
    using Key = sf::Keyboard::Scancode;
    if (sf::Keyboard::isKeyPressed(Key::Up))    speed += 2;
    if (sf::Keyboard::isKeyPressed(Key::Down))  speed -= 2;
    if (sf::Keyboard::isKeyPressed(Key::Left))  cameraX -= 100;
    if (sf::Keyboard::isKeyPressed(Key::Right)) cameraX += 100;
}

void Racer::update() {
    cameraZ += speed;
    if (cameraZ >= 18000) cameraZ = 0;
}

void Racer::render(sf::RenderWindow& window) {
    for (auto& r : roads)
        r.drawItem(window);
    drawNumber(window, score, 10, 10);
}

void Racer::calculateScore(int opIndex, int numIndex) {
    static constexpr char ops[] = "+-*/%";
    char op = ops[opIndex % 5];
    int num = numIndex % 10;

    switch (op) {
        case '+': score += num; break;
        case '-': score -= num; break;
        case '*': score *= num; break;
        case '/': if (num) score /= num; break;
        case '%': if (num) score %= num; break;
    }
}

void Racer::drawNumber(sf::RenderWindow& window, int number, int x, int y) {
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial Unicode.ttf")) {
            std::cerr << "[Error] Font load failed!\n";
            return;
        }
        loaded = true;
    }

    sf::Text text(font, std::to_string(number), 32u);
    text.setFillColor(sf::Color::White);
    text.setPosition({static_cast<float>(x), static_cast<float>(y)});
    window.draw(text);
}