//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_FLOATINGOBJ_H
#define UNDEROCEAN_FLOATINGOBJ_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include "common/utils/Physics_deprecate.h"
namespace sf {
    class Texture;
}
class FloatingObj {
public:
    explicit FloatingObj(const sf::Texture& texture, int bgHeight, float heightScale, float phase_offset, float amplitude, float frequency)
        :bubbleBasePos_(sf::Vector2f(0, static_cast<float>(bgHeight) * heightScale)),   //heightScale means the ratio of height
        phase_offset_(phase_offset), amplitude_(amplitude), frequency_(frequency){
        bubbleSprite_.setTexture(texture);
        bubbleSprite_.setPosition(bubbleBasePos_);
    }
    void update(float dt) {
        static float totalTime = 0.0f;
        totalTime += dt;
        float offset0 = amplitude_ * std::sin(phase_offset_ + totalTime * frequency_ * 2.0f * static_cast<float>(Physics::PI));
        bubbleSprite_.setPosition(bubbleBasePos_.x, bubbleBasePos_.y + offset0);
    }
    void render(sf::RenderWindow& window) const {
        window.draw(bubbleSprite_);
    }
private:
    sf::Vector2f bubbleBasePos_;
    sf::Sprite bubbleSprite_{};
    float phase_offset_;
    float amplitude_;
    float frequency_;
};

#endif //UNDEROCEAN_FLOATINGOBJ_H