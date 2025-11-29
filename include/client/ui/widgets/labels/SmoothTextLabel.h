//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_SMOOTHTEXTLABEL_H
#define UNDEROCEAN_SMOOTHTEXTLABEL_H
#include <iostream>

#include "TextLabel.h"
#include "common/utils/DirectFollower.h"

class SmoothTextLabel : public TextLabel, public DirectFollower {
public:
    explicit SmoothTextLabel(sf::Text&& text, float smoothTime = 1.f, sf::Vector2f target = { 0.f, 0.f })
        :TextLabel(std::move(text)), DirectFollower(target, smoothTime), fakeSize_(getOriginSize()), targetSize_(getOriginSize()),
        fakeText_(text){
    }
    void setBounds(const sf::Vector2f pos, const sf::Vector2f size) override {
        setTarget(pos);
        sizeDiff_ = size - fakeSize_;
        targetSize_ = size;
        TextLabel::setBounds(pos, size);
    }
    void setBoundsWithoutAnim(const sf::Vector2f pos, const sf::Vector2f size) {
        setTargetWithoutAnim(pos);
        TextLabel::setBounds(pos, size);
        fakeSize_ = size;
        targetSize_ = size;
        sizeDiff_ = sf::Vector2f();
    }
    void updateTotal(float dt) {  //total means axis + size
        DirectFollower::update(dt);
        fakeSize_ += sizeDiff_ * dt / smoothTime_;
        sizeDiff_ = targetSize_ - fakeSize_;
    }
    void render(sf::RenderWindow& window) override {
        fakeText_.setPosition(position_);
        fakeText_.setScale(fakeSize_.x / getOriginSize().x, fakeSize_.y / getOriginSize().y);
        if (visible_)
            window.draw(fakeText_);
    }
    void drawTarget(sf::RenderWindow& window) const {
        sf::RectangleShape point(sf::Vector2f(64.f, 64.f));
        point.setPosition(targetPos_);
        point.setFillColor(sf::Color::Red);
        window.draw(point);
    }
    void convertTo(sf::View view) {   //seems to stand still
        sf::Vector2f oldCenter = oldView_.getCenter();
        sf::Vector2f oldSize = oldView_.getSize();
        sf::Vector2f oldWorldMin = pos_;
        sf::Vector2f oldWorldMax = pos_ + size_;
        sf::Vector2f normMin{
            (oldWorldMin.x - (oldCenter.x - oldSize.x / 2.f)) / oldSize.x,
            (oldWorldMin.y - (oldCenter.y - oldSize.y / 2.f)) / oldSize.y
        };
        sf::Vector2f normMax{
            (oldWorldMax.x - (oldCenter.x - oldSize.x / 2.f)) / oldSize.x,
            (oldWorldMax.y - (oldCenter.y - oldSize.y / 2.f)) / oldSize.y
        };
        sf::Vector2f newCenter = view.getCenter();
        sf::Vector2f newSize = view.getSize();
        sf::Vector2f newWorldMin{
            (newCenter.x - newSize.x / 2.f) + normMin.x * newSize.x,
            (newCenter.y - newSize.y / 2.f) + normMin.y * newSize.y
        };
        sf::Vector2f newWorldMax{
            (newCenter.x - newSize.x / 2.f) + normMax.x * newSize.x,
            (newCenter.y - newSize.y / 2.f) + normMax.y * newSize.y
        };
        sf::Vector2f newPos = newWorldMin;
        sf::Vector2f newSizeWorld = newWorldMax - newWorldMin;
        setBoundsWithoutAnim(newPos, newSizeWorld);
    }
    void setOldView(const sf::View& oldView) {
        oldView_ = oldView;
    }
    void setFillColor(const sf::Color& color) {
        text_.setFillColor(color);
        fakeText_.setFillColor(color);
    }
    void setOutlineColor(const sf::Color& color, int thickness) {
        text_.setOutlineThickness(thickness);
        text_.setOutlineColor(color);
        fakeText_.setOutlineThickness(thickness);
        fakeText_.setOutlineColor(color);
    }
private:
    sf::Vector2f sizeDiff_;
    sf::Vector2f fakeSize_;  //DirectFollower::position_ is fake, while TextLabel::pos_ is true!
    sf::Vector2f targetSize_;
    sf::Text fakeText_;
    sf::View oldView_;   //save the view to transform between different scenes, need to be set manually by previous scene
    static sf::Vector2f getViewOrigin(const sf::View& view) {
        return view.getCenter() - view.getSize() / 2.f;
    }
};
#endif //UNDEROCEAN_SMOOTHTEXTLABEL_H