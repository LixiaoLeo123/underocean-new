//
// Created by 15201 on 12/15/2025.
//

#ifndef UNDEROCEAN_LOADINGINDICATOR_H
#define UNDEROCEAN_LOADINGINDICATOR_H
#include "client/ui/widgets/WidgetBase.h"

class LoadingIndicator  : public WidgetBase {
public:
    LoadingIndicator() {
        square.setFillColor(sf::Color::White);;
        square.setSize(sf::Vector2f(10, 10));
    }
    // void setBounds(const sf::Vector2f pos, const sf::Vector2f size) override {
    //     square.setPosition(pos);
    //     square.setSize(size);
    //     square.setFillColor(sf::Color::White);
    // }
    void render(sf::RenderWindow &window) override {
        if (visible_)
            window.draw(square);
    }
    void update(float dt) {
        animTimer_ += dt;
        square.setPosition(
            pos_.x + std::sin(animFreq_ * animTimer_) * size_.x,
            pos_.y
        );
    }
private:
    sf::RectangleShape square;
    constexpr static float animFreq_{2.f};
    float animTimer_{0.f};
};
#endif //UNDEROCEAN_LOADINGINDICATOR_H