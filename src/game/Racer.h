#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include "Road.h"
#include "../engine/DriftEngine.h"

class Racer {
public:
    Racer();
    void handleInput();
    void update();
    void render(sf::RenderWindow& window);

private:
    // 资源
    std::shared_ptr<sf::Texture> bgTexture, itemTexture;
    std::shared_ptr<sf::Sprite> bgSprite, itemSprite;
    std::shared_ptr<sf::SoundBuffer> buffer;
    std::shared_ptr<sf::Sound> sound, bgm;

    // 状态
    std::vector<Road> roads;
    int cameraX = 0, cameraZ = 0, speed = 100, score = 0;
    float cameraY = 1600.f;
    bool isJumping = false;
    std::string expr;

    drift::DriftEngine engine;

    void calculateScore(int opIndex, int numIndex);
    void drawNumber(sf::RenderWindow& window, int number, int x, int y);
    void drawText(sf::RenderWindow& window, const std::string& s, int x, int y);
    void drawHUD(sf::RenderWindow& window);
};