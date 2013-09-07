#pragma once
#include "Config.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

class Drawer {
    sf::RenderTarget& rt;
public:
    Drawer(sf::RenderTarget& rt) : rt(rt) { }

    void drawCircle(Point position, float radius, Color color) {
        static sf::CircleShape shape;
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
        shape.setFillColor(color);
        rt.draw(shape);
    }

    void drawLine(Point a, Point b, float width, Color color) {
        sf::Vertex line[] = { sf::Vertex(a), sf::Vertex(b) };
        rt.draw(line, 2, sf::Lines);
    }
};

enum class ActorType {
    Link,
    SolarPlant,
    Turret,
    Harvester,
    Rock,
    EnergyPacket
};

class Actor {
protected:
    unsigned id;
    Drawer& rt;
    Config& config;

public:
    float energy;
    Point position;

    Actor(Drawer& rt, Config& config, unsigned id, Point const& position)
        : rt(rt), config(config), id(id), position(position)
        , energy(0.f)
    { }

    unsigned getId() const { return id; }
    virtual void draw() = 0;
    virtual ActorType getType() = 0;
};

template<class T>
class Spawner {
    Drawer& rt;
    Config& config;
public:
    Spawner(Drawer& rt, Config& config) : rt(rt), config(config) { }
    T spawn (Point const& info) {
        return T (rt, config, info);
    }
    std::unique_ptr<T> spawn_ptr (Point const& position, unsigned id) {
        return std::unique_ptr<T>(new T (rt, config, id, position));
    }
};

class Turret : public Actor {
public:
    Turret(Drawer& rt, Config& config, unsigned id, Point const& position)
        : Actor(rt, config, id, position)
    { }
    void draw() override {
        rt.drawCircle(position, 10.f, config.get("turret_color", sf::Color::Red));
    }
    ActorType getType() override { return ActorType::Turret; }
};

class EnergyLink : public Actor {
public:
    EnergyLink(Drawer& rt, Config& config, unsigned id, Point const& position)
        : Actor(rt, config, id, position)
    { }
    void draw() override {
        rt.drawCircle(position, 6.f, config.get("link_color", sf::Color(220, 220, 20)));
    }
    ActorType getType() override { return ActorType::Link; }
};

class SolarPlant : public Actor {
public:
    SolarPlant(Drawer& rt, Config& config, unsigned id, Point const& position)
        : Actor(rt, config, id, position)
    { }
    void draw() override {
        rt.drawCircle(position, 30.f, config.get("plant_color", sf::Color(116, 40, 148)));
    }
    ActorType getType() override { return ActorType::SolarPlant; }
};

class Rock : public Actor {
public:
    Rock(Drawer& rt, Config& config, unsigned id, Point const& position)
        : Actor(rt, config, id, position)
    { }
    void draw() override {
        rt.drawCircle(position, 15.f, config.get("rock_color", sf::Color(120, 120, 120)));
    }
    ActorType getType() override { return ActorType::Rock; }
};

class Harvester : public Actor {
public:
    unsigned target;
    Point target_pos;

    Harvester(Drawer& rt, Config& config, unsigned id, Point const& position)
        : Actor(rt, config, id, position)
        , target(0u)
    { }
    void draw() override {
        rt.drawCircle(position, 12.f, config.get("harvester_color", sf::Color(20, 170, 30)));
        if (target != 0) {
            rt.drawLine(position, target_pos, 2.f, sf::Color::Green);
        }
    }
    ActorType getType() override { return ActorType::Harvester; }
};

class EnergyPacket : public Actor {
public:
    unsigned target;

    EnergyPacket(Drawer& rt, Config& config, unsigned id, Point const& position, unsigned target)
        : Actor(rt, config, id, position)
        , target(target)
    { }
    void draw() override {
        rt.drawCircle(position, 3.f, config.get("energypacket_color", sf::Color::Yellow));
    }
    ActorType getType() override { return ActorType::EnergyPacket; }
};

