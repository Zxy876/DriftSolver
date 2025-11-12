#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Road.h"
#include "engine/DriftEngine.h"  // ✅ 通过 target_include_directories(src)，这样写即可

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

    // ---- SFML 资源 (智能指针管理) ----
    std::shared_ptr<sf::Texture> bgTexture;
    std::shared_ptr<sf::Texture> itemTexture;
    std::shared_ptr<sf::Sprite>  bgSprite;
    std::shared_ptr<sf::Sprite>  itemSprite;

    std::shared_ptr<sf::SoundBuffer> buffer;
    std::shared_ptr<sf::Sound> sound;
    std::shared_ptr<sf::Sound> bgm;

    // ---- 相机与物理参数 ----
    int   cameraX = 0, cameraZ = 0, speed = 0;
    float cameraY = 0.f, y = 0.f, dy = 0.f;
    bool  isJumping = false;

    // ---- 表达式/分数 ----
    drift::DriftEngine engine;
    std::string expr = "0";
    int score = 0;

    // ---- 内部逻辑 ----
    void calculateScore(int operatorIndex, int numberIndex);
    void drawNumber(sf::RenderWindow& window, int number, int x, int y);
    void drawText(sf::RenderWindow& window, const std::string& s, int x, int y);
    void drawTrape(sf::RenderWindow& window, sf::Color c,
                   int x1, int y1, int w1, int x2, int y2, int w2);
};