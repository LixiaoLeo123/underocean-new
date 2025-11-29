//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_FOLLOWER_H
#define UNDEROCEAN_FOLLOWER_H
#include <SFML\Graphics.hpp>
class Follower {
public:
    explicit Follower(sf::Vector2f target = {0, 0}, float maxSpeed = 100, float maxAccel = 50)
        : position_(target), velocity_(0, 0), acceleration_(0, 0),
        targetPos_(target), maxSpeed_(maxSpeed), maxAcceleration_(maxAccel) {
    }
    void update(float dt) {
        auto dir = targetPos_ - position_;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) {
            dir /= len;
            acceleration_ = dir * maxAcceleration_;
        }
        else {
            acceleration_ = { 0,0 };
        }
        velocity_ += acceleration_ * dt;
        float speed = std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
        if (speed > maxSpeed_) velocity_ = velocity_ / speed * maxSpeed_;
        position_ += velocity_ * dt;
    }
    void setTarget(sf::Vector2f t) { targetPos_ = t; }
    [[nodiscard]] sf::Vector2f getPosition() const { return position_; }
    [[nodiscard]] sf::Vector2f getVelocity() const { return velocity_; }

protected:
    sf::Vector2f position_, velocity_, acceleration_, targetPos_;
    float maxSpeed_, maxAcceleration_;
};
#endif //UNDEROCEAN_FOLLOWER_H