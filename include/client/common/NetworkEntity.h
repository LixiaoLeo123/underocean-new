// cpp
#ifndef UNDEROCEAN_NETWORKENTITY_H
#define UNDEROCEAN_NETWORKENTITY_H
#include "server/core(deprecate)/GameData.h"
#define EXP_INTERPOLATION
#ifdef EXP_INTERPOLATION
#include <iostream>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "ResourceManager.h"
#include "common/Types.h"

class NetworkEntity {
public:
    NetworkEntity()
    : clientPos_(0.f, 0.f), prevNetPos_(0.f,0.f), netPos_(0.f,0.f),
      velocity_(0.f,0.f),
      interpTimer_(0.f),
      hasNet_(false), hasPrevNet_(false), SERVER_DT_(static_cast<float>(TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC) / static_cast<float>(GameData::SERVER_TPS)) {}
    void render(sf::RenderWindow& window) const {
        window.draw(sprite_);
    }
    // Immediate local placement (no interpolation)
    void setPos(float x, float y) {
        clientPos_.x = x;
        clientPos_.y = y;
        sprite_.setPosition(clientPos_);
        hasNet_ = false;
        hasPrevNet_ = false;
        velocity_ = {0.f, 0.f};
    }
    void setPos(sf::Vector2f pos){ setPos(pos.x, pos.y); }
    // server sends only position and server update interval
    void setNetworkState(const sf::Vector2f& pos, unsigned currentTick) {  //tick to avoid two state in one tick, causing inf velocity
        if (!hasNet_) {
            // first authoritative sample: set both prev and current to same value
            prevNetPos_ = pos;
            netPos_ = pos;
            velocity_ = {0.f, 0.f};
            interpTimer_ = SERVER_DT_;
            hasNet_ = true;
            hasPrevNet_ = false;
            // snap client to server if desired; keep current clientPos_ to avoid pops
            clientPos_ = netPos_;
            sprite_.setPosition(clientPos_);
            return;
        }
        if (lastTick_ != currentTick) {
            if (interpTimer_ > 0.001f)
                velocity_ = (netPos_ - prevNetPos_) / interpTimer_;
            prevNetPos_ = netPos_;
            netPos_ = pos;
            interpTimer_ = 0.f;
        }
        else {
            netPos_ = pos;
            if (interpTimer_ > 0.001f)
                velocity_ = (netPos_ - prevNetPos_) / interpTimer_;
        }
        hasPrevNet_ = true;
        lastTick_ = currentTick;
    }
    // Call each frame with delta time in seconds
    void update(float dt) {
        updatePos(dt);
        updateAngle();
        updateAnim(dt);
    }
    void updatePos(float dt);
    void updateAngle();
    void updateAnim(float dt);
    void setType(EntityTypeID type);
    void setSize(float size);
    [[nodiscard]] sf::Vector2f getPosition() const { return clientPos_; }
    [[nodiscard]] sf::Vector2f getVelocity() const { return velocity_; }
private:
    static sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) {
        return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
    }
    sf::Sprite sprite_;
    sf::Vector2f clientPos_;     // currently drawn position
    sf::Vector2f prevNetPos_;    // previous authoritative position
    sf::Vector2f netPos_;        // latest authoritative position
    sf::Vector2f velocity_;      // derived from consecutive authoritative positions
    float SERVER_DT_;   // server update interval
    float interpTimer_;    // time since last authoritative update
    float animTimer{ 0.f };
    bool hasNet_;
    bool hasPrevNet_;
    int totalFrames_{ -1 };   // decided by type
    unsigned lastTick_{ 0 };
    float frameInterval_{ -1.f }; // time per frame
    unsigned frameWidth_{ 0u };   // decided by type
    unsigned frameHeight_{ 0u };  // decided by type
    constexpr static float ANGLE_UPDATE_SPEED2_THRESHOLD{ 0.3f };
};
inline void NetworkEntity::setType(EntityTypeID type) {
    sprite_.setTexture(ResourceManager::getTexture(getTexturePath(type)));
    totalFrames_ = getTextureTotalFrame(type);
    frameInterval_ = getFrameInterval(type);
    frameHeight_ = sprite_.getTexture()->getSize().y;
    frameWidth_ = sprite_.getTexture()->getSize().x / totalFrames_;   //assume horizontal strip
    sprite_.setOrigin(static_cast<float>(frameWidth_) / 2.f, static_cast<float>(frameHeight_) / 2.f);
}
inline void NetworkEntity::setSize(float size) {
    assert(sprite_.getTexture() && "Texture(type) must be set before setting size");
    float scale = size / static_cast<float>(sprite_.getTexture()->getSize().x);
    sprite_.setScale(scale, scale);
}
inline void NetworkEntity::updatePos(float dt) {
    if (!hasNet_) {
        // no server info: simple prediction by last known velocity
        clientPos_ += velocity_ * dt;
        sprite_.setPosition(clientPos_);
        return;
    }
    if (!hasPrevNet_) {
        // only one sample received: snap to it (or could hold/extrapolate)
        clientPos_ = netPos_;
        sprite_.setPosition(clientPos_);
        return;
    }
    interpTimer_ += dt;
    constexpr float ALPHA = 0.4f;  //smoothing factor, less is smoother
    sf::Vector2f target;
    if (interpTimer_ <= SERVER_DT_) {
        target = prevNetPos_ + (netPos_ - prevNetPos_) * (interpTimer_ / SERVER_DT_);
    }
    else {
        float extra = interpTimer_ - SERVER_DT_;
        target = netPos_ + velocity_ * extra;
    }
    clientPos_ += (target - clientPos_) * ALPHA;
    sprite_.setPosition(clientPos_);
}
inline void NetworkEntity::updateAngle() {
    if (velocity_.x * velocity_.x + velocity_.y * velocity_.y < ANGLE_UPDATE_SPEED2_THRESHOLD) return; // no movement, no rotation
    float angle = std::atan2(velocity_.y, velocity_.x) * 180.f / 3.14159265f;
    float current = sprite_.getRotation();
    float delta = angle - current;
    while (delta > 180.f) delta -= 360.f;
    while (delta < -180.f) delta += 360.f;
    constexpr float alpha = 0.2f;  //less is smoother
    sprite_.setRotation(current + delta * alpha);
}
inline void NetworkEntity::updateAnim(float dt) {
    // update the frame of sprite texture
    animTimer += dt;
    unsigned frame = static_cast<unsigned>(animTimer / frameInterval_) % totalFrames_;
    sprite_.setTextureRect(sf::IntRect(frame * frameWidth_, 0, frameWidth_, frameHeight_));
}
#else

#endif
#endif // UNDEROCEAN_NETWORKENTITY_H
