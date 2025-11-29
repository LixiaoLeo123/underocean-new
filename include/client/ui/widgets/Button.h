//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_BUTTON_H
#define UNDEROCEAN_BUTTON_H

#include <functional>
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include "WidgetBase.h"

namespace sf {
	class Font;
}

class Button : public WidgetBase {
public:
    Button(const sf::Vector2f& pos, const sf::Vector2f& size,
        const std::string& text, const sf::Font& font);
    void render(sf::RenderWindow& window) override {
        sf::Color targetColor;
        if (isPressed_) {
            targetColor = pressedColor_;
        }
        else if (isHovered_) {
            targetColor = hoverColor_;
        }
        else {
            targetColor = normalColor_;
        }
        if (shape_.getFillColor() != targetColor) {
            shape_.setFillColor(targetColor);   //only upload when different
        }
        window.draw(shape_);
        window.draw(text_);
    }
	void setBounds(sf::Vector2f pos, sf::Vector2f size) override {
    	WidgetBase::setBounds(pos, size);
	    shape_.setPosition(pos);
    	shape_.setSize(size);
    	text_.setPosition(pos);
    }
    void handleEvent(const sf::Event& event) override {
		if (event.type == sf::Event::MouseMoved) {
            isHovered_ = isHoveredAt(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
		}
		else if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left && isHovered_) {
				isPressed_ = true;
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				if (isPressed_ && isHovered_ && onClick_) {
					onClick_();
				}
				isPressed_ = false;
			}
		}
    }
    void setOnClick(std::function<void()> callback) {
        onClick_ = std::move(callback);
    }
	void setColor(const sf::Color& normal = sf::Color::Transparent,
		const sf::Color& hover = sf::Color::Transparent,
		const sf::Color& pressed = sf::Color::Transparent) {
		normalColor_ = normal;
		hoverColor_ = hover;
		pressedColor_ = pressed;
	}
    bool isHoveredAt(sf::Vector2f worldMousePos) const {
        return getGlobalBounds().contains(worldMousePos);
    }
    [[nodiscard]] sf::FloatRect getGlobalBounds() const {
        return shape_.getGlobalBounds();
    }
private:
    bool isHoveredAt() const;
    bool isPressed_ = false;
    bool isHovered_ = false;
    sf::RectangleShape shape_;
    sf::Text text_;
    std::function<void()> onClick_;
    sf::Color normalColor_ = sf::Color(0, 0, 0);
    sf::Color hoverColor_ = sf::Color(0, 0, 0);
    sf::Color pressedColor_ = sf::Color(0, 0, 0);
};
#endif //UNDEROCEAN_BUTTON_H