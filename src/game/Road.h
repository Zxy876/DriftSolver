#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>

constexpr int WinWidth = 1024;
constexpr int WinHeight = 768;
constexpr int roadWidth = 1800;
constexpr int roadSegLength = 180;
constexpr int roadCount = 1884;
constexpr int itemSize = 450;
constexpr const char charItem[] = "1234567890+*/-%";

struct Road {
    float x, y, z;
    float X, Y, W;
    float scale, curve;
    sf::Sprite spr;
    int operatorIndex;
    int numberIndex;

    Road(int _x, int _y, int _z, float _c, const sf::Sprite& _spr);
    void generateItem(bool always);
    void project(int camX, int camY, int camZ);
    void drawItem(sf::RenderWindow& window);
};