//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_IMAGEBUTTON_H
#define UNDEROCEAN_IMAGEBUTTON_H
#include <iostream>
#include <SFML/Graphics/Sprite.hpp>

#include "Button.h"

class ImageButton : public WidgetBase{
public:
    ImageButton(const sf::Texture& initTexture, const sf::Texture& hoverTexture, sf::Vector2f pos, sf::Vector2f size, int levelNum)
        :WidgetBase(pos, size), initOriginalSize_(initTexture.getSize()), hoverOriginalSize_(hoverTexture.getSize()), levelNum_(levelNum) {
        init_.setTexture(initTexture);
        hover_.setTexture(hoverTexture);
    }
    void render(sf::RenderWindow& window) override {
        if (isHovered_) {
            window.draw(hover_);
        }
        else {
            window.draw(init_);
        }
    }
    void setBounds(sf::Vector2f pos, sf::Vector2f size) override {
        WidgetBase::setBounds(pos, size);
        init_.setPosition(pos);
        init_.setScale(size.x / initOriginalSize_.x, size.y / initOriginalSize_.y);
        hover_.setPosition(pos);
        hover_.setScale(size.x / hoverOriginalSize_.x, size.y / hoverOriginalSize_.y);
    }
    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::MouseMoved) {
            isHovered_ = isHoveredAt(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left && isHovered_) {
                if (onClick_) onClick_(levelNum_);
            }
        }
    }
    void setOnClick(std::function<void(int levelNum)> callback) {
        onClick_ = std::move(callback);
    }
    bool isHoveredAt(sf::Vector2f worldMousePos) const {
        return getGlobalBounds().contains(worldMousePos);
    }
    void setTexture(const sf::Texture& initTexture, const sf::Texture& hoverTexture) {
        init_.setTexture(initTexture);
        hover_.setTexture(hoverTexture);
        initOriginalSize_ = initTexture.getSize();
        hoverOriginalSize_ = hoverTexture.getSize();
        init_.setScale(size_.x / initOriginalSize_.x, size_.y / initOriginalSize_.y);
        hover_.setScale(size_.x / hoverOriginalSize_.x, size_.y / hoverOriginalSize_.y);
    }
private:
    sf::Sprite init_;
    sf::Sprite hover_;
    sf::Vector2u initOriginalSize_;
    sf::Vector2u hoverOriginalSize_;
    std::function<void(int levelNum)> onClick_;
    int levelNum_;   //for callback
    bool isHoveredAt() const;
    bool isHovered_ = false;
    sf::FloatRect getGlobalBounds() const { return init_.getGlobalBounds();}
};
#endif //UNDEROCEAN_IMAGEBUTTON_H