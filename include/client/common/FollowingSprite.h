//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_FOLLOWINGSPRITE_H
#define UNDEROCEAN_FOLLOWINGSPRITE_H
#include <memory>
#include "InputManager.h"
#include "common/utils/Follower.h"
#include <iostream>
class FollowingSprite : public Follower {
public:
    explicit FollowingSprite(const sf::Texture& texture, sf::Vector2f target = { 0, 0 }, float maxSpeed = 100, float maxAccel = 50, sf::FloatRect range = sf::FloatRect(0.f, 0.f, -1.f, -1.f))
        :Follower(target, maxSpeed, maxAccel) {  //range must be positive! being negative will be regarded as none
        sprite_.setTexture(texture);
        sprite_.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
        sprite_.setPosition(position_);
        velocity_.x = 1.f;  //initial direction
        if (range.width >= 0)
            pRange_ = std::make_unique<sf::FloatRect>(std::move(range));
    }
    void update(float dt) {
        Follower::update(dt);
        sprite_.setPosition(position_);
        const sf::Vector2f mousePos = InputManager::getInstance().mousePosWorld;
        if (pRange_) {
            if (pRange_->contains(mousePos)) {
                setTarget(InputManager::getInstance().mousePosWorld);
            }
        }
    }
    void render(sf::RenderWindow& window) {
        sprite_.setRotation(std::atan2f(velocity_.y, velocity_.x) * 180.f / PI + 180.f);
        window.draw(sprite_);
    }
private:
    static constexpr float PI = 3.1415926f;
    sf::Sprite sprite_;
    std::unique_ptr<sf::FloatRect> pRange_;
};
#endif //UNDEROCEAN_FOLLOWINGSPRITE_H