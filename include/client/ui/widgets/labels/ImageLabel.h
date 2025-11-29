//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_IMAGELABEL_H
#define UNDEROCEAN_IMAGELABEL_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "client/ui/widgets/ILabel.h"



class ImageLabel : public ILabel {
public:
    ImageLabel(const sf::Texture& texture, const sf::Vector2f pos, const sf::Vector2f size)
        :ILabel(pos, size), textureSize_(texture.getSize()) {
        sprite_.setTexture(texture);
    }

    explicit ImageLabel(const sf::Texture& texture)   //bounds will be adjusted automatically by layout
        :textureSize_(texture.getSize()) {
        sprite_.setTexture(texture);
    }
    void setBounds(const sf::Vector2f pos, const sf::Vector2f size) override {
        sprite_.setPosition(pos);
        sprite_.setScale(size.x / textureSize_.x, size.y / textureSize_.y);
    }
    void render(sf::RenderWindow& window) override {
        window.draw(sprite_);
    }
private:
    sf::Sprite sprite_;
    sf::Vector2u textureSize_;
};
#endif //UNDEROCEAN_IMAGELABEL_H