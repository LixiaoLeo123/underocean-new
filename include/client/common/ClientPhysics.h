//
// Created by 15201 on 12/18/2025.
//

#ifndef UNDEROCEAN_CLIENTPHYSICS_H
#define UNDEROCEAN_CLIENTPHYSICS_H
#include <cmath>
#include <SFML/System/Vector2.hpp>

class ClientPhysics {  //for client-only sf::Vector
public:
    static sf::Vector2f makeVec(sf::Vector2f rawVec, float len) {
        float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
        if (rawLen == 0.f) {
            return {0.f, 0.f};
        }
        return sf::Vector2f(rawVec) / rawLen * len;
    }
    static sf::Vector2f clampVec(const sf::Vector2f& rawVec, float len);
};
inline sf::Vector2f ClientPhysics::clampVec(const sf::Vector2f& rawVec, float len) {
    float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
    if (rawLen > len) {
        return sf::Vector2f(rawVec) / rawLen * len;
    }
    return {rawVec};
}
#endif //UNDEROCEAN_CLIENTPHYSICS_H