#pragma once
#include "Config.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

class Actor {
protected:
    unsigned id;
    sf::RenderTarget& rt;
    Config& config;
public:
    Actor(sf::RenderTarget& rt, Config& config) : rt(rt), config(config) { }
    virtual void draw() = 0;
};

template<class T>
class Spawner {
    sf::RenderTarget& rt;
    Config& config;
public:
    Spawner(sf::RenderTarget& rt, Config& config) : rt(rt), config(config) { }
    T spawn (typename T::Info const& info) {
        return T (rt, config, info);
    }
    std::unique_ptr<T> spawn_ptr (typename T::Info const& info) {
        return std::unique_ptr<T>(new T (rt, config, info));
    }
};

class Turret : public Actor {
    sf::CircleShape shape;
public:
    struct Info { 
        sf::Vector2f position;
        Info(sf::Vector2f const& position) : position(position) { }
    };

    Turret(sf::RenderTarget& rt, Config& config, Info const& info)
        : Actor(rt, config)
    {
        shape.setRadius(10.f);
        shape.setPosition(info.position);
        shape.setFillColor(config.get("turret_color", sf::Color::Red));
    }

    void draw() override {
        rt.draw(shape);
    }
};

class EnergyLink : public Actor {
};

class SolarPlant : public Actor {
};

class Rock : public Actor {
    sf::CircleShape shape;
public:
    struct Info {
        sf::Vector2f position;
        Info(sf::Vector2f const& position) : position(position) { }
    };

    Rock(sf::RenderTarget& rt, Config& config, Info const& info)
        : Actor(rt, config)
    {
        //shape.setPointCount(rand() % 4 + 4);
        shape.setRadius(15.f);
        shape.setPosition(info.position);
        /*for (unsigned i = 0; i < shape.getPointCount(); ++i)
            shape.setPoint(i, sf::Vector2f(*/
        shape.setFillColor(config.get("rock_color", sf::Color(120, 120, 120)));
    }
    void draw() override {
        rt.draw(shape);
    }
};

class Harvester : public Actor {
};

