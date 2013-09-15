#pragma once
#include <random>

/*template<typename Integral>
Integral random(Integral min, Integral max) {
    std::default_random_engine engine(std::random_device());
    std::uniform_int_distribution<Integral> dist(min, max);
    return dist(engine);
}*/
int random(int min, int max) {
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine);
}

float random (float min, float max) {
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(engine);
}


