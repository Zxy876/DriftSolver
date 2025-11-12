#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "Road.h"

class Racer {
public:
    Racer();
    void handleInput();
    void update();
    void render(sf::RenderWindow& window);
    bool isRunning() const { return running; }

private:
    bool running = true;

    std::vector<Road> roads;
    sf::Texture bgTexture, itemTexture;
    sf::Sprite bgSprite, itemSprite;
    sf::SoundBuffer buffer[5];
    sf::Sound sound, bgm;
    int cameraX, cameraZ, speed;
    float cameraY, y, dy;
    bool isJumping;
    int score;

    void calculateScore(int operatorIndex, int numberIndex);
    void drawNumber(sf::RenderWindow& window, int number, int x, int y);
    void drawTrape(sf::RenderWindow& window, sf::Color c,
                   int x1, int y1, int w1, int x2, int y2, int w2);
};