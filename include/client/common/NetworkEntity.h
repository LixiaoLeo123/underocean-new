// cpp
#ifndef UNDEROCEAN_NETWORKENTITY_H
#define UNDEROCEAN_NETWORKENTITY_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>

#include "server/core/GameData.h"

class NetworkEntity {
public:
    NetworkEntity()
    : clientPos_(0.f, 0.f), prevNetPos_(0.f,0.f), netPos_(0.f,0.f),
      velocity_(0.f,0.f),
      interpTimer_(0.f),
      hasNet_(false), hasPrevNet_(false), SERVER_DT_(1.f / GameData::SERVER_TPS) {}
    void render(sf::RenderWindow& window) {
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
    // server sends only position and server update interval
    void setNetworkState(const sf::Vector2f& pos) {
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
        // shift the last network sample into previous and set new latest
        prevNetPos_ = netPos_;
        netPos_ = pos;
        // derive velocity from last two authoritative positions
        velocity_ = (netPos_ - prevNetPos_) / SERVER_DT_;
        interpTimer_ = 0.f;
        hasPrevNet_ = true;
    }
    // Call each frame with delta time in seconds
    void update(float dt) {
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
        if (interpTimer_ <= SERVER_DT_) {
            float t = interpTimer_ / SERVER_DT_;
            // smoothstep easing
            float s = t * t * (3.f - 2.f * t);   //smooth
            clientPos_ = lerp(prevNetPos_, netPos_, s);
            sprite_.setPosition(clientPos_);
        } else {
            // updates are late: extrapolate using derived velocity
            float extra = interpTimer_ - SERVER_DT_;
            clientPos_ = netPos_ + velocity_ * extra;
            sprite_.setPosition(clientPos_);
        }
    }
    void setSprite(const sf::Sprite& s) { sprite_ = s; }
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
    bool hasNet_;
    bool hasPrevNet_;
};

#endif // UNDEROCEAN_NETWORKENTITY_H
