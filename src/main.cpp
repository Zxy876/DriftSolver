#include <SFML/Graphics.hpp>
#include "game/Racer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280u, 720u}), "DriftSolver");

    Racer racer;

    while (window.isOpen()) {
        // SFML3: pollEvent 无参数（返回 optional），这里不细分事件类型
        while (window.pollEvent()) {
            // 按 Esc 退出（可选）
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) {
                window.close();
            }
        }

        racer.handleInput();
        racer.update();

        window.clear(sf::Color::Black);
        racer.render(window);
        window.display();
    }
    return 0;
}