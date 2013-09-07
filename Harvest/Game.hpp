#pragma once
#include "Config.hpp"
#include "Random.hpp"
#include <boost/range/adaptor/map.hpp>
#include <boost/lexical_cast.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <set>

float distance (Point const& a, Point const& b) {
    float dx = a.x-b.x;
    float dy = a.y-b.y;
    return std::sqrt(dx*dx + dy*dy);
}
float direction (Point const& a, Point const& b) {
    return std::atan2(b.y-a.y, b.x-a.x);
}

class GameState {
    Config& config;
    sf::RenderTarget& rt;
    Drawer drawer;
    sf::Font font;

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
        font.loadFromFile("../Resources/font.ttf");
        // Add a few rocks
        Spawner<Rock> rockSpawner(drawer, config);
        for (unsigned i = 0; i < 20; ++i) {
            sf::Vector2f pos (random(0.f, 800.f), random(0.f, 600.f));
            objects[++objectIdCounter] = rockSpawner.spawn_ptr(pos, objectIdCounter+1);
        }
    }

    std::string getClickStateAsStr() {
        switch (clickState) {
        case ClickState::Idle: return "Idle";
        case ClickState::BuildTurret: return "BuildTurret";
        case ClickState::BuildHarvester: return "BuildHarvester";
        case ClickState::BuildLink: return "BuildLink";
        case ClickState::BuildPowerplant: return "BuildPowerplant";
        default: return "Invalid state";
        }
    }

    void mouseRelease(sf::Event const& e) {
        sf::Vector2f clickPos (static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));

        switch (clickState) {
            case ClickState::Idle: break;
            case ClickState::BuildTurret: {
                auto turret_cost = config.get("turret_cost", 15);
                if (money >= turret_cost) {
                    money -= turret_cost;
                    Spawner<Turret> turretSpawner(drawer, config);
                    ++objectIdCounter;
                    objects[objectIdCounter] = turretSpawner.spawn_ptr(clickPos, objectIdCounter);
                }
                break; 
            }
            case ClickState::BuildHarvester: {
                auto harvester_cost = config.get("harvester_cost", 12);
                if (money >= harvester_cost) {
                    money -= harvester_cost;
                    Spawner<Harvester> harvesterSpawner(drawer, config);
                    ++objectIdCounter;
                    objects[objectIdCounter] = harvesterSpawner.spawn_ptr(clickPos, objectIdCounter);
                }
                break;
            }
            case ClickState::BuildLink: {
                auto link_cost = config.get("link_cost", 2);
                if (money >= link_cost) {
                    money -= link_cost;
                    Spawner<EnergyLink> linkSpawner(drawer, config);
                    ++objectIdCounter;
                    objects[objectIdCounter] = linkSpawner.spawn_ptr(clickPos, objectIdCounter);
                }
                break;
            }
            case ClickState::BuildPowerplant: {
                auto powerplant_cost = config.get("powerplant_cost", 50);
                if (money >= powerplant_cost) {
                    money -= powerplant_cost;
                    Spawner<SolarPlant> plantSpawner(drawer, config);
                    ++objectIdCounter;
                    objects[objectIdCounter] = plantSpawner.spawn_ptr(clickPos, objectIdCounter);
                }
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
            case sf::Keyboard::T: {
                Spawner<Tank> tankSpawner(drawer, config);
                sf::Vector2f pos (random(0.f, 800.f), random(0.f, 600.f));
                ++objectIdCounter;
                objects[objectIdCounter] = tankSpawner.spawn_ptr(pos, objectIdCounter);
                break;
            }
        }
    }

    void draw() {
        for (auto& object : objects | boost::adaptors::map_values) {
            object->draw();
        }

        // output debug info
        sf::Text text("Selected : " + getClickStateAsStr(), font);
        text.setPosition(0, 0);
        text.setColor(sf::Color(190, 190, 190));
        rt.draw(text);

        text.setPosition(0, 30);
        text.setString("Object count: " + boost::lexical_cast<std::string>(objects.size()));
        rt.draw(text);

        text.setPosition(0, 60);
        text.setString("Money: " + boost::lexical_cast<std::string>(money));
        rt.draw(text);
    }

    std::vector<unsigned> query(Point position, float radius, unsigned exclude, std::set<ActorType> filterType) {
        std::vector<unsigned> results;
        for (auto& iter : objects) {
            if (distance(position, iter.second->position) < radius) {
                if (iter.first != exclude)
                    if (filterType.find(iter.second->getType()) != filterType.end())
                        results.push_back(iter.first);
            }
        }
        return results;
    }

    void update() {
        std::vector<unsigned> flaggedForDeletion;

        std::set<ActorType> possible_targets;
        possible_targets.insert(ActorType::Harvester);
        possible_targets.insert(ActorType::Link);
        possible_targets.insert(ActorType::Turret);
        possible_targets.insert(ActorType::SolarPlant);

        std::set<ActorType> rocks;
        rocks.insert(ActorType::Rock);

        std::set<ActorType> enemies;
        enemies.insert(ActorType::Tank);

        for (auto& object : objects | boost::adaptors::map_values) {
            if (object->getType() == ActorType::SolarPlant) {
                if (object->energy >= 100.f) {
                    // get possible objects we can send our packet to
                    auto neighbours = query(object->position, 100.f, object->getId(), possible_targets);
                    if (!neighbours.empty()) {
                        // pick a random target
                        unsigned n = random(0, neighbours.size()-1);
                        // create a new energy packet
                        auto packet = std::unique_ptr<EnergyPacket>(
                            new EnergyPacket(drawer, config, objectIdCounter+1, object->position, neighbours[n]));
                        objects[++objectIdCounter] = std::move(packet);
                        // start accumulating energy for the next packet
                        object->energy = 0.f;
                    }
                }
                else {
                    object->energy += 2.f;
                }
            }
            else if (object->getType() == ActorType::EnergyPacket) {
                EnergyPacket & p = *dynamic_cast<EnergyPacket*>(object.get());
                auto& target = objects[p.target];
                // reached the target
                if (distance(p.position, target->position) < 5.f) {
                    // Are we terminating or bouncing?
                    bool bounce = false;

                    if ((target->getType() == ActorType::Link) || 
                        (target->getType() == ActorType::SolarPlant)) {
                        bounce = true;
                    }
                    else if ((target->getType() == ActorType::Harvester) &&
                             (target->energy > 0.f)) {
                        bounce = true;
                    }
                    else if ((target->getType() == ActorType::Turret) &&
                            (target->energy >= config.get("max_turret_energy", 20.f))) {
                        bounce = true;
                    }

                    if (bounce) {
                        auto neighbours = query(target->position, 100.f, target->getId(), possible_targets);
                        unsigned n = random(0, neighbours.size()-1);
                        p.target = neighbours[n];
                    }
                    else {
                        // terminate
                        flaggedForDeletion.push_back(p.getId());
                        target->energy += 1.f;
                    }
                }
                else {
                    p.position.x += cosf(direction(p.position, target->position));
                    p.position.y += sinf(direction(p.position, target->position));
                }
            }
            else if (object->getType() == ActorType::Harvester) {
                Harvester& hv = *dynamic_cast<Harvester*>(object.get());
                // is the harvester farming?
                if (hv.target == 0) {
                    if (hv.energy > 0.f) { // if it's not loaded, don't even look around
                        auto neighbours = query(object->position, 100.f, object->getId(), rocks);
                        // any rocks nearby?
                        if (!neighbours.empty()) {
                            unsigned n = random(0, neighbours.size()-1);
                            hv.target = neighbours[n];
                            hv.target_pos = objects[hv.target]->position;
                        }
                    }
                } 
                else {
                    if (hv.energy > 0.f) { // currently farming
                        object->energy -= 0.01f;
                    } 
                    else { // just finished farming
                        object->energy = 0.f;
                        money += 1;
                        hv.target = 0;
                    }
                }
            }
            else if (object->getType() == ActorType::Turret) {
                Turret& turret = *dynamic_cast<Turret*>(object.get());
                
                if (turret.target == 0) {
                    auto neighbours = query(object->position, 200.f, object->getId(), enemies);
                    if (neighbours.empty())
                        continue;
                    unsigned n = random(0, neighbours.size()-1);

                    auto& enemy = *objects[neighbours[n]];
                    turret.target = neighbours[n];
                    turret.target_pos = enemy.position;
                }
                else {
                    auto & result = objects.find(turret.target);
                    if (result == objects.end()) {
                        turret.target = 0;
                        continue;
                    }
                    auto & enemy = *(result->second);
                    // the target might have moved in the meantime
                    turret.target_pos = enemy.position;
                    enemy.energy -= 1.f;
                }
            }
            else if (object->getType() == ActorType::Tank) {
                Tank& tank = *dynamic_cast<Tank*>(object.get());
                if (tank.energy <= 0.f) {
                    flaggedForDeletion.push_back(tank.getId());
                    continue;
                }
                else {
                    if (tank.target == 0) {
                        auto neighbours = query(object->position, 500.f, object->getId(), possible_targets);
                        // any rocks nearby?
                        if (!neighbours.empty()) {
                            unsigned n = random(0, neighbours.size()-1);
                            tank.target = neighbours[n];
                        }
                    }
                    else {
                        auto & result = objects.find(tank.target);
                        if (result == objects.end()) {
                            tank.target = 0;
                            continue;
                        }
                        auto const& target = result->second;
                        tank.position.x += cosf(direction(tank.position, target->position)) * 0.5f;
                        tank.position.y += sinf(direction(tank.position, target->position)) * 0.5f;
                    }
                }
            }
        }

        for (unsigned i : flaggedForDeletion)
            objects.erase(i);
    }
};

