#include <SFML/Graphics.hpp>
#include "game/Racer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280u, 720u}), "DriftSolver", sf::State::Windowed);
    Racer racer;

    while (window.isOpen()) {
        while (auto event = window.pollEvent())
            if (event->is<sf::Event::Closed>()) window.close();

        racer.handleInput();
        racer.update();

        window.clear(sf::Color::Black);
        racer.render(window);
        window.display();
    }
}