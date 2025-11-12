#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

constexpr int   WinWidth      = 1024;
constexpr int   WinHeight     = 768;
constexpr int   roadCount     = 160;
constexpr int   roadSegLength = 200;
constexpr float itemSize      = 400.f;

struct Road {
    Road(int _x, int _y, int _z, float _curve, const sf::Sprite& _spr);
    void generateItem(bool always);
    void project(int camX, int camY, int camZ);
    void drawItem(sf::RenderWindow& window);

    int   X{0}, Y{0}, Z{0};
    float W{0.f};
    float curve{0.f};
    bool  hasItem{false};
    int   operatorIndex{0};
    int   numberIndex{0};
    std::shared_ptr<sf::Sprite> spr;
};