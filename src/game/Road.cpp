#include "Road.h"
#include <cmath>
#include <cstdint>

Road::Road(int _x, int _y, int _z, float _c, const sf::Sprite& _spr)
    : x(_x), y(_y), z(_z), curve(_c), spr(std::make_shared<sf::Sprite>(_spr)) {
    scale = 1.f;
    X = Y = W = 0.f;
    operatorIndex = rand() % 5;
    numberIndex   = rand() % 10;
}

void Road::generateItem(bool always) {
    if (always || (rand() % 3 == 0)) {
        operatorIndex = rand() % 5;
        numberIndex   = rand() % 10;
    }
}

void Road::project(int camX, int camY, int camZ) {
    scale = 0.84f / (z - camZ + 1.0f);
    X = (1 + scale * (x - camX)) * WinWidth / 2;
    Y = (1 - scale * (y - camY)) * WinHeight / 2;
    W = scale * roadWidth * WinWidth / 2;
}

void Road::drawItem(sf::RenderWindow& window) {
    if (!spr) return;

    float s = scale * 0.3f;
    spr->setScale({s, s}); // ✅ SFML 3 setScale(Vector2f)

    auto bounds = spr->getGlobalBounds();
    spr->setPosition({ X - bounds.size.x / 2.f, Y - bounds.size.y });

    spr->setColor(sf::Color(255, 255, 255,
                            static_cast<uint8_t>(255 * scale)));

    window.draw(*spr);
}