//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_TEXTLABEL_H
#define UNDEROCEAN_TEXTLABEL_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include "client/ui/widgets/ILabel.h"

class  TextLabel : public ILabel{
protected:
    sf::Text text_;
    sf::Vector2f originSize;
public:
    explicit TextLabel(sf::Text&& text)    //must setBounds!!
        :text_(text),
        originSize(text.getLocalBounds().width, text.getLocalBounds().height){
    }
    void setBounds(const sf::Vector2f pos, const sf::Vector2f size) override {
        WidgetBase::setBounds(pos, size);
        text_.setPosition(pos);
        text_.setScale(size.x / originSize.x, size.y / originSize.y);
    }
    sf::Vector2f getSize() const override { return {originSize.x * text_.getScale().x, originSize.y * text_.getScale().y}; }
    sf::Vector2f calculateSizeByWidth(float width) const {return {width, width / getRatio()};}
    sf::Vector2f getOriginSize() const { return originSize; }
    float getRatio() const { return originSize.x / originSize.y; }
    sf::Text& get() { return text_; }  //dangerous
    void render(sf::RenderWindow& window) override {
        if (visible_)
            window.draw(text_);
    }
};
#endif //UNDEROCEAN_TEXTLABEL_H