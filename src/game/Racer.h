#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>
#include <vector>
#include <string>
#include "Road.h"
#include "../engine/DriftEngine.h"

class Racer {
public:
    Racer();
    void handleInput();
    void update();
    void render(sf::RenderWindow& window);

private:
    // 资源（可选）
    std::shared_ptr<sf::Texture> carTexture;
    std::shared_ptr<sf::Sprite>  carSprite;

    // 道路
    std::vector<Road> roads;

    // 摄像机/车辆状态
    float cameraX = 0.f;
    float cameraY = 1600.f;
    int   cameraZ = 0;
    int   speed   = 120;        // 越大滚动越快
    float driftPhase = 0.f;
    float carY = 600.f;
    float carX = 0.f;

    // 尾迹
    struct TrailPt { float x, y; };
    std::deque<TrailPt> trail;

    // 粒子（运算拾取时）
    struct Particle {
        sf::Vector2f pos, vel;
        sf::Color    color;
        float        life;
        char         symbol;
    };
    std::vector<Particle> particles;

    // 表达式 + 计算引擎
    std::string  expr = "0";
    int          score = 0;
    drift::DriftEngine engine;

    // HUD 字体（可选）
    sf::Font font;
    bool     fontReady = false;

private:
    void drawRoad(sf::RenderWindow& w);
    void drawCar(sf::RenderWindow& w);
    void drawTrail(sf::RenderWindow& w);
    void drawParticles(sf::RenderWindow& w);
    void drawHUD(sf::RenderWindow& w);
    void calculateScore(int opIndex, int numIndex);
};