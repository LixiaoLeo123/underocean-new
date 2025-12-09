//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_ENTITY_H
#define UNDEROCEAN_ENTITY_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

#include "server/core(deprecate)/CollisionBox.h"


enum class EntityType {
    FISH,
    PLAYER,
    OBJECT,
    ENEMY,
    NEUTRAL
};
enum class ExAggLevel {
    KIND,
    NEUTRAL,
    DANGEROUS
};
class OldEntity {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rotation;
    float size;    //approximate radius
    sf::Sprite sprite;
    sf::Clock animationClock;
    int currentFrame{0};
    sf::IntRect textureRect;
    std::vector<CollisionBox> boxes;
    ExAggLevel aggLevel = ExAggLevel::NEUTRAL;
    int cachedRow{-1};
    int cachedCol{-1};
    int cachedROWS{-1};
    int cachedCOLS{-1};
    bool deleted{false};    //remove if eaten of out of border
    float maxVelocity = -1.f;
    OldEntity(sf::Vector2f pos, float size, sf::Vector2f velocity = {}, float rotation = 0.0f, int ROWS = -1, int COLS = -1)
        : position(pos), size(size), velocity(velocity), rotation(rotation), cachedROWS(ROWS), cachedCOLS(COLS){}
    virtual ~OldEntity() = default;
    virtual void update(const std::vector<std::vector<std::vector<OldEntity*>>>& grid, float dt) = 0;  //if true then remove
    virtual void updateAsPlayer(float dt, sf::RenderWindow& window) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};
#endif //UNDEROCEAN_ENTITY_H