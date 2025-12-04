//
// Created by 15201 on 11/18/2025.
//
#include "server/game/creatures/SmallFish.h"

#include <array>

#include "common/utils/Physics_deprecate.h"
#include "server/core/GameData.h"

const std::array<SmallFish::Params, static_cast<size_t>(SmallFishType::COUNT)>& SmallFish::getParamsTable() {
    static const std::array<Params, static_cast<size_t>(SmallFishType::COUNT)> table = { {
        {//0
            100.0f,   // neighborRadius2
            50.0f,     // separationRadius2
            300.0f,   // avoidRadius2
            20.0f,       // cohesionWeight
            1000.0f,       // separationWeight
            100.0f,       // alignmentWeight
            2.0f,       // avoidWeight
            10.0f,      // maxVelocity
            30,         // spriteWidth
            12,          // spriteHeight
            "images/smallfish/smallfish0.png",     //textureId
            2,           //minSize
            10           //maxSize
        },
        {//1
            70.0f,
            50.0f,
            600.0f,
            15.0f,
            1000.0f,
            100.0f,
            5.0f,
            40.0f,
            30,
            12,
            "images/smallfish/smallfish1.png",
            2,
            10
        },
        {//2
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            12,
            6,
            "images/smallfish/smallfish2.png",
            2,
            10
        },
        {//3
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            54,
            22,
            "images/smallfish/smallfish3.png",
            2,
            10
        },
        {//4
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            20,
            12,
            "images/smallfish/smallfish4.png",
            2,
            10
        },
        {//5
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            26,
            12,
            "images/smallfish/smallfish5.png",
            2,
            10
        },
        {//6
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            28,
            24,
            "images/smallfish/smallfish6.png",
            2,
            10
        },
        {//7
            25000.0f,
            1600.0f,
            40000.0f,
            0.3f,
            0.8f,
            1.0f,
            3.0f,
            15.0f,
            16,
            12,
            "images/smallfish/smallfish7.png",
            2,
            10
        }
    } };
    return table;
}
void SmallFish::update(const std::vector<std::vector<std::vector<OldEntity*>>>& grid, float dt) {
    sf::Vector2f cohesion(0, 0);
    sf::Vector2f separation(0, 0);
    sf::Vector2f alignment(0, 0);
    sf::Vector2f avoid(0, 0);
    int neighborCount = 0;
    auto& p = getParams();   //params
    maxVelScale = 0.4f;
    for (int dr = -GameData::PERCEPTION_DIST; dr <= GameData::PERCEPTION_DIST; ++dr) {  //only tick nearby entities
        for (int dc = -GameData::PERCEPTION_DIST; dc <= GameData::PERCEPTION_DIST; ++dc) {
            int r = cachedRow + dr;
            int c = cachedCol + dc;
            if (r >= 0 && r < cachedROWS && c >= 0 && c < cachedCOLS) {
                for (OldEntity* pEntity : grid[r][c]) {
                    assert(pEntity);
                    if (pEntity == static_cast<OldEntity *>(this)) continue;
                    if (SmallFish* other = dynamic_cast<SmallFish*>(pEntity)) {
                        if (other->type == type) {
                            float dist2 = Physics::distance2(this->position, other->position);
                            if (dist2 < p.neighborRadius2) {   //group
                                cohesion += other->position;
                                alignment += other->velocity;
                                ++neighborCount;
                                if (dist2 < p.separationRadius2)   //separate
                                    separation += (this->position - other->position) / dist2;
                            }
                        }
                    }
                    else if (pEntity->aggLevel == ExAggLevel::DANGEROUS) {
                        maxVelScale = 1.0;   //if big fish exist, then speed = maxspeed(default less than maxspeed)
                        float dist2 = Physics::distance2(this->position, pEntity->position);
                        if (dist2 < p.avoidRadius2)
                            avoid += (this->position - pEntity->position) / dist2;
                    }
                }
            }
        }
    }
    if (neighborCount) {
        cohesion /= static_cast<float>(neighborCount);
        alignment /= static_cast<float>(neighborCount);
    }
    cohesion -= position;
    sf::Vector2f rawVelocity =
        cohesion * p.cohesionWeight +
        separation * p.separationWeight +
        alignment * p.alignmentWeight +
        avoid * p.avoidWeight;
    velocity = Physics::clampVec(rawVelocity, maxVelocity * maxVelScale);
    //std::cout << Physics::len2Of(rawVelocity) << "\t" << Physics::len2Of(velocity);
    position += velocity * dt;
    sprite.setPosition(position);
    rotation = std::atan2(velocity.y, velocity.x);
    sprite.setRotation(rotation * 180.f / Random::PI_F);
    //std::cout << rotation << std::endl;
    animTime += dt;   //animation start
    constexpr float frameDuration = 0.3f;
    const int currentFrame = static_cast<int>(animTime / frameDuration) % 2;
    const auto& params = getParams();
    textureRect.left = currentFrame * params.spriteWidth;
    sprite.setTextureRect(textureRect);
}
void SmallFish::updateAsPlayer(float dt, sf::RenderWindow& window) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
    sf::Vector2f dir = mouseWorld - position;
    float distance2 = dir.x * dir.x + dir.y * dir.y;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        const float ACCEL = distance2;
        velocity += dir * ACCEL * dt;
        velocity = Physics::clampVec(velocity, maxVelocity * 2);
    }
    else
        velocity -= 0.3f * dt * velocity;
    position += velocity * dt;
    if (position.x < 0.f) {  //keep in border
        position.x = 0.f;
        velocity.x = 0.f;
    }
    else if (position.x > cachedWIDTH) {
        position.x = cachedWIDTH;
        velocity.x = 0.f;
    }

    if (position.y < 0.f) {
        position.y = 0.f;
        velocity.y = 0.f;
    }
    else if (position.y > cachedHEIGHT) {
        position.y = cachedHEIGHT;
        velocity.y = 0.f;
    }
    sprite.setPosition(position);
    rotation = std::atan2(velocity.y, velocity.x);
    sprite.setRotation(rotation * 180.f / Random::PI_F);
    animTime += dt;   //animation start
    constexpr float frameDuration = 0.3f;
    const int currentFrame = static_cast<int>(animTime / frameDuration) % 2;
    const auto& params = getParams();
    textureRect.left = currentFrame * params.spriteWidth;
    sprite.setTextureRect(textureRect);
}