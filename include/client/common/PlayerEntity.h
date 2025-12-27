//
// Created by 15201 on 12/8/2025.
//

#ifndef UNDEROCEAN_PLAYERENTITY_H
#define UNDEROCEAN_PLAYERENTITY_H
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "ClientPhysics.h"
#include "ResourceManager.h"
#include "common/Types.h"
#include "common/utils/Physics.h"

class PlayerEntity {
public:
    void setType(EntityTypeID type);
    void setSize(float size);
    void triggerHurtFlash(float strength = 1.f) {
        hurtFlash_ = std::min(1.f, hurtFlash_ + strength);
    }
    void setMaxVec(float maxVec) {
        maxVelocity_ = maxVec;
        resistanceAccPerVel_ = maxAcceleration_ / std::pow(maxVelocity_, DEFAULT_RESISTANCE_FORCE_POW_TIMES);
        overspeedResistanceAccPerVel_ = maxAcceleration_ / std::pow(maxVelocity_, OVERSPEED_RESISTANCE_FORCE_POW_TIMES);
    }
    void setMaxAcc(float maxAcc) {
        maxAcceleration_ = maxAcc;
        resistanceAccPerVel_ = maxAcceleration_ / std::pow(maxVelocity_, DEFAULT_RESISTANCE_FORCE_POW_TIMES);
        overspeedResistanceAccPerVel_ = maxAcceleration_ / std::pow(maxVelocity_, OVERSPEED_RESISTANCE_FORCE_POW_TIMES);
    }
    void render(sf::RenderWindow& window) const {
        if (hurtFlash_ > 0.f && hurtShader_) {
            hurtShader_->setUniform("u_flash", hurtFlash_);
            sf::RenderStates states;
            states.shader = hurtShader_;
            window.draw(sprite_, states);
        } else {
            window.draw(sprite_);
        }
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
    void dash(float dashVel) {
        dashVel_ = dashVel;
    }
    //for gore
    [[nodiscard]] const sf::Sprite& getSprite() const { return sprite_; }
    [[nodiscard]] const sf::Vector2f& getScale() const { return sprite_.getScale(); }
    [[nodiscard]] float getSize() const { return cachedSize; }
private:
    static constexpr float DEFAULT_RESISTANCE_FORCE_POW_TIMES = 2.f;  //f = k*v^?
    static constexpr float OVERSPEED_RESISTANCE_FORCE_POW_TIMES = 0.5f;  //f = k*v^?
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
    float resistanceAccPerVel_{ 0.1f };  //matching MaxVelocity
    float overspeedResistanceAccPerVel_{ 0.1f };  //matching MaxVelocity
    float dashVel_ { -1.f };  //-1.f means no dash
    float cachedSize{};
    bool isFlipped{false};
    //hurt flash
    float hurtFlash_ = 0.f;  //current intensity
    float hurtFlashDecay_ = 6.f;  //decreasing rate
    sf::Shader* hurtShader_ { &ResourceManager::getShader("shaders/hurtflash.frag") };
    float calcResistanceAcc(float vel) const;
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
inline float PlayerEntity::calcResistanceAcc(float vel) const {
    if (vel > maxVelocity_)
        return overspeedResistanceAccPerVel_ * std::pow(vel, OVERSPEED_RESISTANCE_FORCE_POW_TIMES);
    return resistanceAccPerVel_ * std::pow(vel, DEFAULT_RESISTANCE_FORCE_POW_TIMES);
}
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
    if (cachedSize == size) return;  //no need to update
    cachedSize = size;
    float scale = size / static_cast<float>(sprite_.getTexture()->getSize().x / totalFrames_);
    sprite_.setScale(scale, scale * (isFlipped ? -1.f : 1.f));
    GameData::playerSize[GameData::playerType] = size;
}
inline void PlayerEntity::update(float dt, sf::Vector2f rawAcc) {
    // when mouse click, move the player to that direction(velocity proportional to distance)
    // player entity should have acceleration and velocity, smoothly move to target position
    // max acceleration and max velocity can be got by ParamTable<EntityTypeID>::MAX_FORCE/VELOCITY
    // and camera should follow the player smoothly, using GameData::CAMERA_ALPHA
    sf::Vector2f inputAcc = ClientPhysics::clampVec(rawAcc, maxAcceleration_);
    if (velocity_.x != 0.f || velocity_.y != 0.f) {
        float currentSpeed = std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
        float dragMagnitude = calcResistanceAcc(currentSpeed) * dt;
        if (dragMagnitude > currentSpeed) {
            velocity_ = {0.f, 0.f};
        } else {
            float reductionRatio = 1.f - (dragMagnitude / currentSpeed);
            velocity_ *= reductionRatio;
        }
    }
    velocity_ += inputAcc * dt;
    if ((rawAcc.x != 0.f || rawAcc.y != 0.f) && dashVel_ > 0.f) {  //dash
        velocity_ += ClientPhysics::makeVec(rawAcc, dashVel_);
        dashVel_ = -1.f;  //reset dash
    }
    constexpr float MIN_SPEED = 0.001f;
    if (velocity_.x * velocity_.x + velocity_.y * velocity_.y < MIN_SPEED * MIN_SPEED) {
        if (inputAcc.x == 0.f && inputAcc.y == 0.f) {
            velocity_ = {0.f, 0.f};
        }
    }
    if (hurtFlash_ > 0.f) {
        hurtFlash_ -= hurtFlashDecay_ * dt;
        if (hurtFlash_ < 0.f) hurtFlash_ = 0.f;
    }
    // velocity_ = {0.7f * maxVelocity_, maxVelocity_ * 0.7f};
    position_ += velocity_ * dt * 1.6f;  //1.5 is speedup factor
    adjustPosInBorder();
    sprite_.setPosition(position_);
    updateAnim(dt);
    updateAngle();
}
inline void PlayerEntity::updateAngle() {
    if (velocity_.y * velocity_.y + velocity_.x * velocity_.x < 0.01f) {
        return;  //not moving
    }
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