#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <memory>

#include <boost/range/adaptor/map.hpp>

#include "Config.hpp"
#include "Actor.hpp"
#include "Random.hpp"

int main(void)
{
    // Window
    int winSizeX = 800, winSizeY = 600;
    sf::RenderWindow window (sf::VideoMode(winSizeX, winSizeY), "");
    sf::Clock timer;

    // Config
    Config globalConfig;
    globalConfig.set("player_hp", 100);
    globalConfig.set("bg_color", sf::Color::Black);
    globalConfig.set("turret_color", sf::Color(255, 106, 106));

    // Game objects
    Spawner<Turret> turretSpawner(window, globalConfig);
    std::map<unsigned, std::unique_ptr<Actor>> objects;
    unsigned objectIdCounter = 0;
    
    // spawn a few rocks
    Spawner<Rock> rockSpawner(window, globalConfig);
    for (unsigned i = 0; i < 20; ++i) {
        sf::Vector2f pos (random(0.f, 800.f), random(0.f, 600.f));
        objects[++objectIdCounter] = rockSpawner.spawn_ptr(pos);
    }


    float delta = 0.f;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            case sf::Event::MouseButtonPressed:
                break;
            case sf::Event::MouseButtonReleased: {
                sf::Vector2f clickPos (static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                objects[++objectIdCounter] = turretSpawner.spawn_ptr(clickPos);
                break; }
                //case sf::Event::MouseMoved:
                //case sf::Event::MouseWheelMoved:
            case sf::Event::Closed:
                return 0;
            }
        }

        // realtime input
        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player.move(Direction::Up);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player.move(Direction::Left);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player.move(Direction::Right);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player.move(Direction::Down);*/

        // DRAWING
        window.clear(sf::Color::White);
        for (auto& object : objects | boost::adaptors::map_values) {
            object->draw();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        window.display();
    }
}