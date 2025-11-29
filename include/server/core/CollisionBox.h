//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_COLLISIONBOX_H
#define UNDEROCEAN_COLLISIONBOX_H
#include <array>
#include <cmath>
#include <SFML/System/Vector2.hpp>
struct CollisionBox {
    sf::Vector2f offset;
    sf::Vector2f size;
    float rotation = 0.0f;   //by radians
    inline std::array<sf::Vector2f, 4> getWorldVertices(const sf::Vector2f entityPos, const float entityRot) const {
        float totalRot = entityRot + rotation;
        float cosR = std::cos(totalRot);
        float sinR = std::sin(totalRot);
        float hw = size.x * 0.5f;
        float hh = size.y * 0.5f;
        std::array<sf::Vector2f, 4> local = {
            sf::Vector2f(-hw, -hh),
            sf::Vector2f(hw, -hh),
            sf::Vector2f(hw, hh),
            sf::Vector2f(-hw, hh)
        };
        sf::Vector2f boxCenterWorld(
            entityPos.x + offset.x * std::cos(entityRot) - offset.y * std::sin(entityRot),
            entityPos.y - offset.x * std::sin(entityRot) + offset.y * std::cos(entityRot)
        );
        std::array<sf::Vector2f, 4> world;          // rotate
        for (int i = 0; i < 4; ++i) {
            sf::Vector2f p = local[i];
            world[i] = sf::Vector2f(
                boxCenterWorld.x + p.x * cosR - p.y * sinR,
                boxCenterWorld.y - p.x * sinR + p.y * cosR
            );
        }
        return world;
    }
};

#endif //UNDEROCEAN_COLLISIONBOX_H