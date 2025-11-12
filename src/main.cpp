#include <SFML/Graphics.hpp>
#include "game/Racer.h"
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode({1024u, 768u}), "DriftSolver");
    std::string title = "DriftSolver 🚗💨"; // 避免 Cocoa nil 传参
    window.setTitle(sf::String::fromUtf8(title.begin(), title.end()));

    Racer racer;

    while (window.isOpen()) {
        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) window.close();
        }
        racer.handleInput();
        racer.update();

        window.clear(sf::Color(30, 20, 60));
        racer.render(window);
        window.display();
    }
}