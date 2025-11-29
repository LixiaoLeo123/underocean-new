//
// Created by 15201 on 11/18/2025.
//
#include "client/ui/widgets/TextButton.h"

class WidgetBase;
TextButton::TextButton(const sf::Vector2f& pos, float height,
               sf::Text&& text, bool shake)
    : text_(text), WidgetBase(pos, sf::Vector2f(height * text.getLocalBounds().width / text.getLocalBounds().height, height)),
    shake_(shake){
    auto bounds = text_.getLocalBounds();
    text_.setScale(height / bounds.width, height / bounds.height);
    bounds = text_.getLocalBounds();
    text_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    text_.setPosition(pos);
    fakeText_ = text_;
    originSize_ = bounds.getSize();
}
TextButton::TextButton(const sf::Vector2f& pos,
               sf::Text&& text, bool shake)
    : text_(text), WidgetBase(pos, sf::Vector2f(text.getLocalBounds().width , text.getLocalBounds().height)),
    shake_(shake){
    auto bounds = text_.getLocalBounds();
    text_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    text_.setPosition(pos);
    fakeText_ = text_;
    originSize_ = bounds.getSize();
}

bool TextButton::isHoveredAt() const {
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition());
    return getGlobalBounds().contains(mousePos);
}