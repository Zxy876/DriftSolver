#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

constexpr int WinWidth      = 1024;
constexpr int WinHeight     = 768;
constexpr int roadWidth     = 1800;
constexpr int roadSegLength = 180;
constexpr int roadCount     = 1884;
constexpr int itemSize      = 450;
constexpr const char charItem[] = "1234567890+*/-%";

struct Road {
    float x = 0.f, y = 0.f, z = 0.f;
    float X = 0.f, Y = 0.f, W = 0.f;
    float scale = 1.f, curve = 0.f;

    std::shared_ptr<sf::Sprite> spr;   // ✅ SFML3 友好

    int operatorIndex = 0;
    int numberIndex   = 0;

    Road(int _x, int _y, int _z, float _c, const sf::Sprite& _spr);
    void generateItem(bool always);
    void project(int camX, int camY, int camZ);
    void drawItem(sf::RenderWindow& window);
};