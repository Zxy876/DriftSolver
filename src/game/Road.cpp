#include "Road.h"
#include <cstdlib>
#include <cmath>

Road::Road(int _x, int _y, int _z, float _curve, const sf::Sprite& _spr)
    : X(_x), Y(_y), Z(_z), curve(_curve) {
    spr = std::make_shared<sf::Sprite>(_spr);
    generateItem(true);
}

void Road::generateItem(bool always) {
    operatorIndex = rand() % 5;
    numberIndex   = rand() % 10;
    hasItem       = always || (rand() % 10 < 2);
}

void Road::project(int camX, int camY, int camZ) {
    // 简化版透视映射
    const float scale = 0.84f;
    int dz = Z - camZ;
    if (dz <= 0) dz += roadCount * roadSegLength;
    float invZ = 1.0f / static_cast<float>(dz);
    X = static_cast<int>((camX + curve * 1000.0f) * invZ * scale);
    Y = static_cast<int>(camY * invZ * scale + 400.0f);
    W = WinWidth * invZ * scale;
}

void Road::drawItem(sf::RenderWindow& window) {
    if (!hasItem || !spr) return;
    spr->setPosition({static_cast<float>(X) - W / 2.f, static_cast<float>(Y) - W / 2.f});
    spr->setScale({W / itemSize, W / itemSize});
    window.draw(*spr);
}