#include <SFML/Graphics.hpp>
#include "game/Racer.h"

int main() {
    // ✅ SFML 3.0: VideoMode 接受 Vector2u，而非两个整数
    sf::RenderWindow window(sf::VideoMode({1024u, 768u}), "DriftSolver");
    window.setFramerateLimit(60);

    Racer racer;

    // ✅ SFML 3.0: pollEvent() 返回 std::optional<Event>
    while (window.isOpen()) {
        while (auto eventOpt = window.pollEvent()) {
            const sf::Event& event = *eventOpt;

            // ✅ 检测窗口关闭事件
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
        }

        racer.handleInput();
        racer.update();

        window.clear(sf::Color(30, 30, 40));
        racer.render(window);
        window.display();
    }

    return 0;
}