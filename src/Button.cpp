//
// Created by 15201 on 11/18/2025.
//
#include "client/ui/widgets/Button.h"
#include <SFML/Graphics.hpp>
class WidgetBase;
Button::Button(const sf::Vector2f& pos, const sf::Vector2f& size,
               const std::string& text, const sf::Font& font)
    : ::WidgetBase(pos, size) {
    shape_.setSize(size);
    shape_.setPosition(pos);
    shape_.setFillColor(normalColor_);
    shape_.setOutlineThickness(1.f);
    shape_.setOutlineColor(sf::Color::White);
    text_.setFont(font);
    text_.setString(text);
    text_.setCharacterSize(18);
    text_.setFillColor(sf::Color::White);
    auto bounds = text_.getLocalBounds();
    text_.setPosition(pos);
}

bool Button::isHoveredAt() const {
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition());
    return getGlobalBounds().contains(mousePos);
}