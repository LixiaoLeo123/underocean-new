//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_TEXTBUTTON_H
#define UNDEROCEAN_TEXTBUTTON_H
#include <functional>
#include <iostream>
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "WidgetBase.h"
#include "common/utils/Random.h"

namespace sf {
	class Font;
}

class TextButton : public WidgetBase {   //similar to button, but no rect
public:
    TextButton(const sf::Vector2f& pos, float height,   //IMPORTANT: pos refers to the center, different from other widgets
        sf::Text&& text, bool shake = false);
	TextButton(const sf::Vector2f& pos,   //without auto scale
		sf::Text&& text, bool shake = false);
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
        if (fakeText_.getFillColor() != targetColor) {
            fakeText_.setFillColor(targetColor);   //only upload when different
        }
    	if (shake_) {
			updateShake();
    	}
        window.draw(fakeText_);
    }
	void updateShake() {
    	sf::Vector2f offset = Random::randUnitVector() * Random::randFloat(0.0f, shakeMoveIntensity_);
    	fakeText_.setPosition(text_.getPosition() + offset);
    	float angleOffset = Random::randFloat(-shakeAngleIntensity_, shakeAngleIntensity_); // ±2度
    	fakeText_.setRotation(text_.getRotation() + angleOffset);
    }
    void handleEvent(const sf::Event& event) override {
		if (event.type == sf::Event::MouseMoved) {
            isHovered_ = isHoveredAt(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
		}
		else if (event.type == sf::Event::MouseButtonPressed) {
			//std::cout << (event.mouseButton.button) << " " << sf::Mouse::Left << std::endl;
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
	void setColor(const sf::Color& normal = sf::Color::White,
		const sf::Color& hover = sf::Color::White,
		const sf::Color& pressed = sf::Color::White) {
		normalColor_ = normal;
		hoverColor_ = hover;
		pressedColor_ = pressed;
	}
    bool isHoveredAt(sf::Vector2f worldMousePos) const {
        return getGlobalBounds().contains(worldMousePos);
    }
	void setBounds(sf::Vector2f pos, sf::Vector2f size) override {
	    WidgetBase::setBounds(pos, size);
    	text_.setPosition(pos);
    	text_.setScale(size.x / originSize_.x, size.y / originSize_.y);
    	fakeText_.setScale(size.x / originSize_.x, size.y / originSize_.y);
    	fakeText_.setPosition(text_.getPosition());
    };
    [[nodiscard]] sf::FloatRect getGlobalBounds() const {
        return text_.getGlobalBounds();
    }
private:
    constexpr static float shakeMoveIntensity_ = 2.f;
	constexpr static float shakeAngleIntensity_ = 2.f;
    bool isHoveredAt() const;
    bool isPressed_ = false;
    bool isHovered_ = false;
	bool shake_;
    sf::Text text_;
	sf::Text fakeText_;
	sf::Vector2f originSize_;   //the text itself
    std::function<void()> onClick_;
    sf::Color normalColor_ = sf::Color(0, 0, 0);
    sf::Color hoverColor_ = sf::Color(0, 0, 0);
    sf::Color pressedColor_ = sf::Color(0, 0, 0);
};
#endif //UNDEROCEAN_TEXTBUTTON_H