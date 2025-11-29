//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_DIRECTFOLLOWER_H
#define UNDEROCEAN_DIRECTFOLLOWER_H
#include <SFML/Graphics.hpp>
class DirectFollower {
public:
    explicit DirectFollower(sf::Vector2f target = { 0.f, 0.f }, float smoothTime = 1.f)
        : position_(target), velocity_(0.f, 0.f), targetPos_(target),
        smoothTime_(smoothTime), omega_(2.f / smoothTime) {
    }
    /*void update(float dt) {
        if (!posDirty_) return;
        sf::Vector2f diff = position_ - targetPos_;
        sf::Vector2f temp = velocity_ + diff * omega_;
        sf::Vector2f change = (velocity_ + temp * dt) * (dt * omega_);
        velocity_ = velocity_ - change;
        position_ = position_ - change;
        if (std::abs(position_.x - targetPos_.x) < 0.01f &&
            std::abs(position_.y - targetPos_.y) < 0.01f) {
            position_ = targetPos_;
            velocity_ = { 0.f, 0.f };
            posDirty_ = false;
        }
    }*/
    void update(float dt) {
        if (!posDirty_) return;
        float alpha = 1.0f - std::exp(-dt / smoothTime_);
        position_ = position_ * (1.0f - alpha) + targetPos_ * alpha;
        if (std::abs(position_.x - targetPos_.x) < 0.01f &&
            std::abs(position_.y - targetPos_.y) < 0.01f) {
            position_ = targetPos_;
            velocity_ = { 0, 0 };
            posDirty_ = false;
        }
    }
    void setSmoothTime(float smoothTime) {
        smoothTime_ = smoothTime;
        omega_ = 2.f / smoothTime;
    }
    void setTarget(sf::Vector2f target) {
        targetPos_ = target;
        posDirty_ = true;
    }
    void setTargetWithoutAnim(sf::Vector2f target) {
        targetPos_ = target;
        velocity_ = sf::Vector2f(0, 0);
        position_ = target;
        posDirty_ = false;
    }
    sf::Vector2f getPosition() const { return position_; }
    sf::Vector2f getVelocity() const { return velocity_; }

private:
    sf::Vector2f velocity_;
    float omega_;
    bool posDirty_ = false;  //update needed?
protected:
    float smoothTime_;   //small == fast
    sf::Vector2f position_;
    sf::Vector2f targetPos_;
};
#endif //UNDEROCEAN_DIRECTFOLLOWER_H