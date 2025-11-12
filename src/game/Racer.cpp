#include "Racer.h"
#include <cmath>
#include <iostream>

Racer::Racer() {
    itemTexture = std::make_shared<sf::Texture>();
    bgTexture   = std::make_shared<sf::Texture>();
    buffer      = std::make_shared<sf::SoundBuffer>();

    itemSprite = std::make_shared<sf::Sprite>(*itemTexture);
    bgSprite   = std::make_shared<sf::Sprite>(*bgTexture);
    sound      = std::make_shared<sf::Sound>(*buffer);
    bgm        = std::make_shared<sf::Sound>(*buffer);

    cameraY = 1600;
    speed   = 100;
    score   = 0;
    expr    = "0";
    engine.clear();

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

    // 漂移震动
    if (speed > 200) {
        cameraX += std::sin(cameraZ * 0.05f) * 50.f;
        cameraY += std::cos(cameraZ * 0.03f) * 10.f;
    } else {
        cameraY = 1600;
    }

    // 表达式拾取逻辑
    if (cameraZ % 900 == 0 && !roads.empty()) {
        int idx = (cameraZ / 180) % roads.size();
        calculateScore(roads[idx].operatorIndex, roads[idx].numberIndex);
    }
}

void Racer::render(sf::RenderWindow& window) {
    for (auto& r : roads) r.drawItem(window);
    drawHUD(window);
}

void Racer::calculateScore(int opIndex, int numIndex) {
    static constexpr char ops[] = "+-*/%";
    char op = ops[opIndex % 5];
    int num = numIndex % 10;
    expr += op;
    expr += std::to_string(num);
    try {
        score = static_cast<int>(std::round(engine.evaluate(expr)));
    } catch (...) { }
}

void Racer::drawNumber(sf::RenderWindow& window, int number, int x, int y) {
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial Unicode.ttf")) return;
        loaded = true;
    }
    sf::Text text(font, std::to_string(number), 32u);
    text.setFillColor(sf::Color::White);
    text.setPosition({(float)x, (float)y});
    window.draw(text);
}

void Racer::drawText(sf::RenderWindow& window, const std::string& s, int x, int y) {
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial Unicode.ttf")) return;
        loaded = true;
    }
    sf::Text text(font, s, 24u);
    text.setFillColor(sf::Color::White);
    text.setPosition({(float)x, (float)y});
    window.draw(text);
}

void Racer::drawHUD(sf::RenderWindow& window) {
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        font.openFromFile("/System/Library/Fonts/Supplemental/Arial Unicode.ttf");
        loaded = true;
    }

    sf::Text exprText(font, expr, 28u);
    exprText.setFillColor(sf::Color(255, 255, 255, 220));
    exprText.setPosition({40.f, 40.f});

    sf::Text resultText(font, "= " + std::to_string(score), 36u);
    resultText.setFillColor(sf::Color(180, 255, 200, 255));
    resultText.setPosition({40.f, 80.f});

    float offset = std::sin(cameraZ * 0.01f) * 5.f;
    exprText.move({0.f, offset});
    resultText.move({0.f, -offset});

    window.draw(exprText);
    window.draw(resultText);
}