//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_RANDOM_H
#define UNDEROCEAN_RANDOM_H
#include <cassert>
#include <random>
#include <SFML/System/Vector2.hpp>

class Random {
public:
    static std::mt19937& generator();
    static float randFloat(float min, float max);
    static int randInt(int min, int max);
    static float randRaid();
    static sf::Vector2f randUnitVector();
    template<typename T>
    static T randType();   //random from a enum
    static constexpr float PI_F = 3.1415926f;
};
inline std::mt19937& Random::generator() {
    static std::mt19937 gen{ std::random_device{}() };
    return gen;
}
inline float Random::randFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, std::nextafter(max, std::numeric_limits<float>::max()));
    return dist(generator());
}
inline int Random::randInt(int min, int max) {
    assert(min <= max);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator());
}
inline sf::Vector2f Random::randUnitVector() {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI_F);
    float angle = angleDist(generator());
    return { std::cos(angle), std::sin(angle) };
}
inline float Random::randRaid() {
    return randFloat(0.0f, 2.0f * PI_F);
}
template<typename T>
inline T Random::randType() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(T::COUNT - 1));
    return static_cast<T>(dist(generator()));
}
#endif //UNDEROCEAN_RANDOM_H