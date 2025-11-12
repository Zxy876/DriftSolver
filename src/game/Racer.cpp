#include "Racer.h"
#include <cmath>
#include <iostream>

static bool tryLoadSystemFont(sf::Font& font) {
    // 尝试加载常见系统字体（macOS）
    const char* mac1 = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
    const char* mac2 = "/System/Library/Fonts/Supplemental/Arial.ttf";
    if (font.openFromFile(mac1)) return true;
    if (font.openFromFile(mac2)) return true;
    return false;
}

Racer::Racer() {
    // 车贴图（可选）
    carTexture = std::make_shared<sf::Texture>();
    if (carTexture->loadFromFile("src/assets/car.png")) {
        carSprite = std::make_shared<sf::Sprite>(*carTexture);
        carSprite->setOrigin({carTexture->getSize().x / 2.f, carTexture->getSize().y / 2.f});
        carSprite->setScale({0.25f, 0.25f});
    } else {
        carSprite = nullptr; // 使用几何图形代替
    }

    // 字体（可选）
    fontReady = tryLoadSystemFont(font);

    // item 占位贴图（白方块）
    sf::Image img({static_cast<unsigned>(itemSize), static_cast<unsigned>(itemSize)}, sf::Color::White);
    auto itemTexture = std::make_shared<sf::Texture>();
    (void)itemTexture->loadFromImage(img); // [[nodiscard]]，不影响运行

    auto itemSprite = std::make_shared<sf::Sprite>(*itemTexture);
    itemSprite->setOrigin({itemSize/2.f, itemSize/2.f});

    // 生成道路
    roads.reserve(roadCount);
    for (int i = 0; i < roadCount; ++i) {
        float c = std::sin(i * 0.1f) * 0.5f;
        roads.emplace_back(0, 0, (i + 1) * roadSegLength, c, *itemSprite);
    }
}

void Racer::handleInput() {
    using Key = sf::Keyboard::Scancode;
    if (sf::Keyboard::isKeyPressed(Key::Left))  { cameraX -= 100; carX -= 3.f; }
    if (sf::Keyboard::isKeyPressed(Key::Right)) { cameraX += 100; carX += 3.f; }
    if (sf::Keyboard::isKeyPressed(Key::Up))    { speed += 2; }
    if (sf::Keyboard::isKeyPressed(Key::Down))  { speed -= 2; }
    if (speed < 0) speed = 0;
}

void Racer::update() {
    cameraZ += speed;
    if (cameraZ >= roadCount * roadSegLength) cameraZ -= roadCount * roadSegLength;

    driftPhase += speed * 0.002f;
    carX += std::sin(driftPhase) * 0.6f;

    // 尾迹
    trail.push_front({512.f + carX, carY - 10.f});
    if (trail.size() > 60) trail.pop_back();

    // 粒子
    for (auto& p : particles) {
        p.pos += p.vel;
        p.life -= 0.02f;
        if (p.life < 0) p.life = 0;
        p.color.a = static_cast<uint8_t>(255 * std::max(0.f, p.life));
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
                                   [](const Particle& p){ return p.life <= 0.f; }),
                    particles.end());

    // 每过若干 road 段，吸收一次运算符 & 数字到表达式
    if (cameraZ % (roadSegLength * 10) == 0 && !roads.empty()) {
        int idx = (cameraZ / roadSegLength) % static_cast<int>(roads.size());
        calculateScore(roads[idx].operatorIndex, roads[idx].numberIndex);
    }
}

void Racer::calculateScore(int opIndex, int numIndex) {
    static constexpr char ops[] = "+-*/%";
    char op = ops[opIndex % 5];
    int  num = numIndex % 10;

    expr.push_back(op);
    expr += std::to_string(num);

    try {
        score = static_cast<int>(std::round(engine.evaluate(expr)));
    } catch (...) {
        // 表达式暂不完整无所谓
    }

    // 粒子颜色按符号变
    sf::Color c = sf::Color::White;
    switch (op) {
        case '+': c = sf::Color( 70, 220, 120); break;
        case '-': c = sf::Color( 80, 200, 255); break;
        case '*': c = sf::Color(200,  80, 220); break;
        case '/': c = sf::Color(255, 230, 120); break;
        case '%': c = sf::Color(255, 120, 120); break;
    }
    for (int i = 0; i < 16; ++i) {
        Particle p;
        p.pos = {512.f + carX, carY - 10.f};
        p.vel = { static_cast<float>((rand()%100 - 50)/40.0),
                  static_cast<float>((rand()%100 - 50)/60.0) };
        p.color = c;
        p.life  = 1.0f;
        p.symbol= op;
        particles.push_back(p);
    }
}

void Racer::drawRoad(sf::RenderWindow& w) {
    // 画一堆横线，模拟向远处收敛
    const int start    = cameraZ / roadSegLength;
    const int maxView  = 60;
    const float camYv  = cameraY + 1000.f;

    for (int n = start; n < start + maxView; ++n) {
        Road& seg = roads[n % roads.size()];
        seg.project(cameraX, static_cast<int>(camYv), cameraZ);

        float t = static_cast<float>(n - start) / maxView;
        sf::Color col(
            static_cast<uint8_t>( 40 + 40 * t),
            static_cast<uint8_t>( 20 + 20 * (1 - t)),
            static_cast<uint8_t>(120 + 80 * (1 - t)),
            255
        );

        sf::RectangleShape line({static_cast<float>(WinWidth), 6.f});
        line.setFillColor(col);
        line.setPosition({0.f, static_cast<float>(seg.Y)});
        w.draw(line);

        // 让道具随段一起画
        seg.drawItem(w);
    }
}

void Racer::drawCar(sf::RenderWindow& w) {
    if (carSprite) {
       carSprite->setRotation(sf::degrees(std::sin(driftPhase) * 15.f));
        carSprite->setPosition({512.f + carX, carY});
        w.draw(*carSprite);
    } else {
        // 三角形代替
        sf::ConvexShape tri(3);
        tri.setPoint(0, { 0.f, -30.f});
        tri.setPoint(1, {-20.f,  20.f});
        tri.setPoint(2, { 20.f,  20.f});
        tri.setFillColor(sf::Color(240, 240, 240));
        tri.setOutlineColor(sf::Color(40, 40, 40));
        tri.setOutlineThickness(2.f);
        tri.setPosition({512.f + carX, carY});
        tri.setRotation(sf::degrees(std::sin(driftPhase) * 15.f));
        w.draw(tri);
    }
}

void Racer::drawTrail(sf::RenderWindow& w) {
    if (trail.size() < 2) return;

    sf::VertexArray arr(sf::PrimitiveType::LineStrip, trail.size());
    for (size_t i = 0; i < trail.size(); ++i) {
        float alpha = static_cast<float>(255 - std::min<size_t>(i*4, 240));
        arr[i].position = { trail[i].x, trail[i].y };
        arr[i].color    = sf::Color(255, 255, 255, static_cast<uint8_t>(alpha));
    }
    w.draw(arr);
}

void Racer::drawParticles(sf::RenderWindow& w) {
    if (particles.empty()) return;

    if (!fontReady) {
        // 没有字体就用小圆点代替
        for (auto& p : particles) {
            sf::CircleShape c(3.f);
            c.setFillColor(p.color);
            c.setPosition(p.pos);
            w.draw(c);
        }
        return;
    }

    for (auto& p : particles) {
        sf::Text t(font, sf::String(std::string(1, p.symbol)), 20u);
        t.setFillColor(p.color);
        t.setPosition(p.pos);
        w.draw(t);
    }
}

void Racer::drawHUD(sf::RenderWindow& w) {
    if (!fontReady) return;

    // 表达式
    sf::String exprStr = sf::String::fromUtf8(expr.begin(), expr.end());
    sf::Text exprText(font, exprStr, 26u);
    exprText.setFillColor(sf::Color::White);
    exprText.setPosition({20.f, 20.f});
    w.draw(exprText);

    // 结果
    std::string r = "= " + std::to_string(score);
    sf::String rStr = sf::String::fromUtf8(r.begin(), r.end());
    sf::Text resText(font, rStr, 34u);
    resText.setFillColor(sf::Color(255, 200, 140));
    resText.setPosition({20.f, 56.f});
    w.draw(resText);
}

void Racer::render(sf::RenderWindow& window) {
    drawRoad(window);
    drawTrail(window);
    drawParticles(window);
    drawCar(window);
    drawHUD(window);
}