//
// Created by 15201 on 11/19/2025.
//

#ifndef UNDEROCEAN_PLAYERIDLABEL_H
#define UNDEROCEAN_PLAYERIDLABEL_H
#include <iostream>

#include "client/ui/widgets/labels/TextLabel.h"
#include "common/utils/Random.h"

class PlayerIdLabel : public TextLabel {
public:
    explicit PlayerIdLabel(sf::Text&& text)
        :TextLabel(std::move(text)), textStr_(text_.getString()) {
    }
    void setShakeState(bool state) {shakeState_ = state;}
    void setShakeIntensity(float intensity){shakeIntensity_ = intensity;}
    void addShakeIntensity(float intensity){shakeIntensity_ += intensity;}
    void append(char c) {   //need to adjust its bound, always middle;nope
        if (textStr_.length() < 16) {
            textStr_ += c;
            text_.setString(textStr_);
        }
        if (textStr_.empty()) return;
    }
    void backspace() {   //adjust bound too
        if (!textStr_.empty()) {
            textStr_.pop_back();
            text_.setString(textStr_);
        }
        if (textStr_.empty()) return;
    }
    bool isEmpty() const {return textStr_.empty();}
    void resetBounds() { //useless
        sf::Vector2f center = getPos() + getSize() / 2.f;
        sf::FloatRect bounds = text_.getLocalBounds();
        float scaleY = text_.getScale().y;
        float scaleX = scaleY * getRatio();
        sf::Vector2f newSize(bounds.width * scaleX, bounds.height * scaleY);
        sf::Vector2f newPos(
            center.x - newSize.x / 2.f - bounds.left * scaleX,
            center.y - newSize.y / 2.f - bounds.top * scaleY
        );
        setBounds(newPos, newSize);
    }
    void render(sf::RenderWindow &window) override {
        if (shakeState_){
            updateShake();
        }
        TextLabel::render(window);
    }
    void updateShake() {
        sf::Vector2f offset = Random::randUnitVector() * Random::randFloat(0.0f, shakeIntensity_);
        text_.setPosition(text_.getPosition() + offset);
        float angleOffset = Random::randFloat(-shakeIntensity_, shakeIntensity_); // ±2度
        text_.setRotation(text_.getRotation() + angleOffset);
    }
    std::string getStr() { return textStr_; }
private:
    std::string textStr_;
    bool shakeState_;
    float shakeIntensity_ = 0.f;
};
#endif //UNDEROCEAN_PLAYERIDLABEL_H