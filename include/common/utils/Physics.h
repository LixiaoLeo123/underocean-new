//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_PHYSICS_H
#define UNDEROCEAN_PHYSICS_H
#include <cmath>

#include "server/new/component/Components.h"

class Physics {
public :
    static constexpr float PI = 3.1415926f;
    static float distance(UVector a, UVector b);
    static float distance2(UVector a, UVector b);
    static sf::Vector2f makeVec(float angleRad, float len) {
        return {std::cos(angleRad) * len, std::sin(angleRad) * len};
    }
    static sf::Vector2f makeVec(sf::Vector2f rawVec, float len) {
        float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
        if (rawLen == 0.f) {
            return {0.f, 0.f};
        }
        return sf::Vector2f(rawVec) / rawLen * len;
    }
    static sf::Vector2f clampVec(const sf::Vector2f& rawVec, float len);
};
inline float Physics::distance(UVector a, UVector b) {
    float diffX = a.x - b.x;
    float diffY = a.y - b.y;
    return std::sqrt(diffX * diffX + diffY * diffY);
}
inline float Physics::distance2(UVector a, UVector b) {
    float diffX = a.x - b.x;
    float diffY = a.y - b.y;
    return (diffX * diffX + diffY * diffY);
}
inline sf::Vector2f Physics::clampVec(const sf::Vector2f& rawVec, float len) {
    float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
    if (rawLen > len) {
        return sf::Vector2f(rawVec) / rawLen * len;
    }
    return {rawVec};
}

#endif //UNDEROCEAN_PHYSICS_H