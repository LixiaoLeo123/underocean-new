//
// Created by 15201 on 12/8/2025.
//

#ifndef UNDEROCEAN_PLAYERENTITY_H
#define UNDEROCEAN_PLAYERENTITY_H
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "ResourceManager.h"
#include "common/Types.h"
#include "common/utils/Physics.h"

class PlayerEntity {
public:
    void setType(EntityTypeID type);
    void setSize(float size);
    void setMaxVec(float maxVec) { maxVelocity_ = maxVec; }
    void setMaxAcc(float maxAcc) { maxAcceleration_ = maxAcc; }
    void render(sf::RenderWindow& window) const {
        window.draw(sprite_);
    }
    void update(float dt, sf::Vector2f acc);
    void setPos(float x, float y) {  //directly set pos
        position_ = { x, y };
        velocity_ = { 0.f, 0.f };
        sprite_.setPosition(position_);
    }
    void updateAngle();
    void updateAnim(float dt);
    [[nodiscard]] float getMaxAcceleration() const { return maxAcceleration_; };
    [[nodiscard]] sf::Vector2f getPosition() const {
        return position_;
    };
    [[nodiscard]] sf::Vector2f getVelocity() const {
        return velocity_;
    }
    void setBorder(float width, float height) {
        mapWidth_ = width;
        mapHeight_ = height;
    }
private:
    sf::Sprite sprite_;
    int totalFrames_{ -1 };   // decided by type
    float frameInterval_{ -1.f }; // time per frame
    unsigned frameWidth_{ 0u };   // decided by type
    unsigned frameHeight_{ 0u };  // decided by type
    sf::Vector2f position_ {};
    sf::Vector2f velocity_ {};
    float maxAcceleration_ { 0.f };
    float maxVelocity_ { 0.f };
    float mapWidth_ { 0.f };
    float mapHeight_ { 0.f };
    bool isFlipped{false};
    void adjustPosInBorder() {   //keep in border
        if (position_.x < 0.f) {
            position_.x = 0.f;
            if (velocity_.x < 0.f) velocity_.x = 0.f;
        }
        if (position_.y < 0.f) {
            position_.y = 0.f;
            if (velocity_.y < 0.f) velocity_.y = 0.f;
        }
        if (position_.x > mapWidth_) {
            position_.x = mapWidth_;
            if (velocity_.x > 0.f) velocity_.x = 0.f;
        }
        if (position_.y > mapHeight_) {
            position_.y = mapHeight_;
            if (velocity_.y > 0.f) velocity_.y = 0.f;
        }
    }
    static float getMaxAcceleration(EntityTypeID id) {
        switch (id) {
#define X(name) case EntityTypeID::name: return ParamTable<EntityTypeID::name>::MAX_FORCE;
            PLAYER_ENTITY_TYPES
            default: return 0.f;
#undef X
        }
    }
    static float getMaxVelocity(EntityTypeID id) {
        switch (id) {
#define X(name) case EntityTypeID::name: return ParamTable<EntityTypeID::name>::MAX_VELOCITY;
            PLAYER_ENTITY_TYPES
            default: return 0.f;
#undef X
        }
    }
};
inline void PlayerEntity::setType(EntityTypeID type) {
    sprite_.setTexture(ResourceManager::getTexture(getTexturePath(type)));
    totalFrames_ = getTextureTotalFrame(type);
    frameInterval_ = getFrameInterval(type);
    frameHeight_ = sprite_.getTexture()->getSize().y;
    frameWidth_ = sprite_.getTexture()->getSize().x / totalFrames_;   //assume horizontal strip
    sprite_.setOrigin(static_cast<float>(frameWidth_) / 2.f, static_cast<float>(frameHeight_) / 2.f);
}
inline void PlayerEntity::setSize(float size) {
    assert(sprite_.getTexture() && "Texture(type) must be set before setting size");
    float scale = size / static_cast<float>(sprite_.getTexture()->getSize().x);
    sprite_.setScale(scale, scale);
}
inline void PlayerEntity::update(float dt, sf::Vector2f rawAcc) {
    // when mouse click, move the player to that direction(velocity proportional to distance)
    // player entity should have acceleration and velocity, smoothly move to target position
    // max acceleration and max velocity can be got by ParamTable<EntityTypeID>::MAX_FORCE/VELOCITY
    // and camera should follow the player smoothly, using GameData::CAMERA_ALPHA
    velocity_ += Physics::clampVec(rawAcc, maxAcceleration_) * dt;
    velocity_ = Physics::clampVec(velocity_, maxVelocity_);
    position_ += velocity_ * dt * 3.f;
    adjustPosInBorder();
    sprite_.setPosition(position_);
    updateAnim(dt);
    updateAngle();
}
inline void PlayerEntity::updateAngle() {
    float angle = std::atan2(velocity_.y, velocity_.x) * 180.f / 3.14159265f;
    float current = sprite_.getRotation();
    float delta = angle - current;
    while (delta > 180.f) delta -= 360.f;
    while (delta < -180.f) delta += 360.f;
    constexpr float alpha = 0.4f;  //less is smoother
    float newRot = current + delta * alpha;
    sprite_.setRotation(newRot);
    bool shouldFlip = newRot >= 90.f && newRot <= 270.f || newRot <= -90.f && newRot >= -270.f;
    if (shouldFlip && !isFlipped) {
        isFlipped = true;
        sf::Vector2f scale = sprite_.getScale();
        scale.y = -abs(scale.y);
        sprite_.setScale(scale);
    } else if (!shouldFlip && isFlipped) {
        isFlipped = false;
        sf::Vector2f scale = sprite_.getScale();
        scale.y = abs(scale.y);
        sprite_.setScale(scale);
    }
}
inline void PlayerEntity::updateAnim(float dt) {
    // update the frame of sprite texture
    // example:
    static float animTimer = 0.f;
    animTimer += dt;
    int frame = static_cast<int>(animTimer / frameInterval_) % totalFrames_;
    sprite_.setTextureRect(sf::IntRect(frame * frameWidth_, 0, frameWidth_, frameHeight_));
}
#endif //UNDEROCEAN_PLAYERENTITY_H