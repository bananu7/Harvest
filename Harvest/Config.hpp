#pragma once
#include <map>
#include <string>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

typedef boost::variant<float, int, std::string, bool, sf::Color> Option;

class Config {
    std::map<std::string, Option> data;
public:
    template<typename T>
    T get(const std::string& name, T const& default_value) {
        auto it = data.find(name);
        if (it == data.end())
            return default_value;

        auto tryPtr = boost::get<T>(&(it->second));
        if (!tryPtr)
            return default_value;

        return *tryPtr;
    }

    void set(const std::string& name, Option value) {
        data[name] = value;
    }
};

