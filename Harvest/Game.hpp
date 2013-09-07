#pragma once
#include "Config.hpp"
#include <boost/range/adaptor/map.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class GameState {
    Config& config;
    sf::RenderTarget& rt;
    Drawer drawer;

    std::map<unsigned, std::unique_ptr<Actor>> objects;
    unsigned objectIdCounter;
public:
    // game score data
    unsigned money;

    enum class ClickState {
        Idle,
        BuildTurret,
        BuildHarvester,
        BuildLink,
        BuildPowerplant
    } clickState;

    GameState(sf::RenderTarget& rt, Config& config)
        : rt(rt)
        , config(config)
        , money(100u)
        , clickState(ClickState::Idle)
        , objectIdCounter(0u)
        , drawer(rt)
    {
        // Add a few rocks
        Spawner<Rock> rockSpawner(drawer, config);
        for (unsigned i = 0; i < 20; ++i) {
            sf::Vector2f pos (random(0.f, 800.f), random(0.f, 600.f));
            objects[++objectIdCounter] = rockSpawner.spawn_ptr(pos);
        }
    }

    std::string getClickStateAsStr() {
        switch (clickState) {
        case ClickState::Idle: return "Idle";
        case ClickState::BuildTurret: return "BuildTurret";
        case ClickState::BuildHarvester: return "BuildHarvester";
        case ClickState::BuildLink: return "BuildLink";
        case ClickState::BuildPowerplant: return "BuildPowerplant";
        }
    }

    void mouseRelease(sf::Event const& e) {
        sf::Vector2f clickPos (static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));

        switch (clickState) {
            case ClickState::Idle: break;
            case ClickState::BuildTurret: {
                Spawner<Turret> turretSpawner(drawer, config);
                objects[++objectIdCounter] = turretSpawner.spawn_ptr(clickPos);
                break; 
            }
            case ClickState::BuildHarvester: {
                Spawner<Harvester> harvesterSpawner(drawer, config);
                objects[++objectIdCounter] = harvesterSpawner.spawn_ptr(clickPos);
                break;
            }
            case ClickState::BuildLink: {
                Spawner<EnergyLink> linkSpawner(drawer, config);
                objects[++objectIdCounter] = linkSpawner.spawn_ptr(clickPos);
                break;
            }
            case ClickState::BuildPowerplant: {
                Spawner<SolarPlant> plantSpawner(drawer, config);
                objects[++objectIdCounter] = plantSpawner.spawn_ptr(clickPos);
                break;
            }
        }
    }
    void mousePress(sf::Event const& e) {
        if (e.mouseButton.button == sf::Mouse::Button::Right) {
            clickState = ClickState::Idle;
        }
    }

    void keyPressed(sf::Event const& e) {
        switch (e.key.code) {
            case sf::Keyboard::D: clickState = ClickState::BuildTurret; break;
            case sf::Keyboard::R: clickState = ClickState::BuildHarvester; break;
            case sf::Keyboard::E: clickState = ClickState::BuildLink; break;
            case sf::Keyboard::S: clickState = ClickState::BuildPowerplant; break;
        }
    }

    void draw() {
        for (auto& object : objects | boost::adaptors::map_values) {
            object->draw();
        }
    }
};

