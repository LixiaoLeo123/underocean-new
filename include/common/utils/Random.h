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
    static float fastRandFloat(float min, float max);
    static int randInt(int min, int max);  //include min and max
    static int fastRandInt(int min, int max);
    static float randRaid();
    static sf::Vector2f randUnitVector();
    static sf::Vector2f fastRandUnitVector();
    template<typename T>
    static T randType();   //random from a enum
    static constexpr float PI_F = 3.1415926f;
};
inline std::mt19937& Random::generator() {
    static std::mt19937 gen{ std::random_device{}() };
    return gen;
}
inline float fastRandFloat(float min, float max) {
    static std::uint32_t seed = 123456789;
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    float r = static_cast<float>(seed) * 2.32830644e-10f;
    return min + r * (max - min);
}
inline float Random::fastRandFloat(float min, float max) {
    return randFloat(min, max);
}
inline int Random::randInt(int min, int max) {
    assert(min <= max);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator());
}
inline int Random::fastRandInt(int min, int max) {
    return randInt(min, max);
}
inline sf::Vector2f Random::randUnitVector() {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI_F);
    float angle = angleDist(generator());
    return { std::cos(angle), std::sin(angle) };
}
inline sf::Vector2f fastRandomUnitVector() {
    static uint32_t s = 0x12345678u;
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    float x = (static_cast<float>(s & 0xFFFF) / 32767.0f) * 2.f - 1.f;
    float y = (static_cast<float>(s >> 16) / 32767.0f) * 2.f - 1.f;
    if (x == 0.f && y == 0.f) y = 1.f;
    float lenSq = x * x + y * y;  //quake 3 fast inv sqrt
    float half = 0.5f * lenSq;
    uint32_t i = *reinterpret_cast<uint32_t *>(&lenSq);
    i = 0x5f3759df - (i >> 1);
    float inv = *reinterpret_cast<float *>(&i);
    inv = inv * (1.5f - half * inv * inv);  //1-time newton iterate
    return { x * inv, y * inv };
}
inline float Random::randRaid() {
    return randFloat(0.0f, 2.0f * PI_F);
}
template<typename T>
T Random::randType() {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(T::COUNT - 1));
    return static_cast<T>(dist(generator()));
}
#endif //UNDEROCEAN_RANDOM_H