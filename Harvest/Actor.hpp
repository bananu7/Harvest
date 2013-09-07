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
};

class Actor {
protected:
    unsigned id;
    Drawer& rt;
    Config& config;
    Point position;

public:
    Actor(Drawer& rt, Config& config, Point const& position)
        : rt(rt), config(config), position(position) { }
    virtual void draw() = 0;
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
    std::unique_ptr<T> spawn_ptr (Point const& position) {
        return std::unique_ptr<T>(new T (rt, config, position));
    }
};

class Turret : public Actor {
public:
    Turret(Drawer& rt, Config& config, Point const& position)
        : Actor(rt, config, position)
    { }
    void draw() override {
        rt.drawCircle(position, 10.f, config.get("turret_color", sf::Color::Red));
    }
};

class EnergyLink : public Actor {
public:
    EnergyLink(Drawer& rt, Config& config, Point const& position)
        : Actor(rt, config, position)
    { }
    void draw() override {
        rt.drawCircle(position, 6.f, config.get("link_color", sf::Color(220, 220, 20)));
    }
};

class SolarPlant : public Actor {
public:
    SolarPlant(Drawer& rt, Config& config, Point const& position)
        : Actor(rt, config, position)
    { }
    void draw() override {
        rt.drawCircle(position, 30.f, config.get("plant_color", sf::Color(116, 40, 148)));
    }
};

class Rock : public Actor {
public:
    Rock(Drawer& rt, Config& config, Point const& position)
        : Actor(rt, config, position)
    { }
    void draw() override {
        rt.drawCircle(position, 15.f, config.get("rock_color", sf::Color(120, 120, 120)));
    }
};

class Harvester : public Actor {
public:
    Harvester(Drawer& rt, Config& config, Point const& position)
        : Actor(rt, config, position)
    { }
    void draw() override {
        rt.drawCircle(position, 12.f, config.get("harvester_color", sf::Color(20, 170, 30)));
    }
};

