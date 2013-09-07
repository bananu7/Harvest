#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <map>
#include <memory>

#include "Config.hpp"
#include "Actor.hpp"
#include "Random.hpp"
#include "Game.hpp"

int main(void)
{
    // Window
    int winSizeX = 800, winSizeY = 600;
    sf::RenderWindow window (sf::VideoMode(winSizeX, winSizeY), "");
    sf::Clock timer;
    float delta = 0.f;

    // Config
    Config globalConfig;
    globalConfig.set("player_hp", 100);
    globalConfig.set("bg_color", sf::Color::Black);
    globalConfig.set("turret_color", sf::Color(255, 106, 106));
    globalConfig.set("turret_cost", 15);
    globalConfig.set("harvester_cost", 10);
    globalConfig.set("link_cost", 2);
    globalConfig.set("powerplant_cost", 50);
    globalConfig.set("max_turret_energy", 20);

    // Game objects
    GameState game(window, globalConfig);

    // main loop
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                game.keyPressed(event);
                break;
            case sf::Event::MouseButtonPressed:
                game.mousePress(event);
                break;
            case sf::Event::MouseButtonReleased:
                game.mouseRelease(event);
                break;
                //case sf::Event::MouseMoved:
                //case sf::Event::MouseWheelMoved:
            case sf::Event::Closed:
                return 0;
            }
        }

        game.update();

        // DRAWING
        window.clear(sf::Color(30,30,30));
        game.draw();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        window.display();
    }
}