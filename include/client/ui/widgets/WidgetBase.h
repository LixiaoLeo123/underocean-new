//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_WIDGETBASE_H
#define UNDEROCEAN_WIDGETBASE_H
#include "client/ui/IWidget.h"

class WidgetBase : public IWidget {   //abstract
public:
    explicit WidgetBase(const sf::Vector2f pos = sf::Vector2f(), const sf::Vector2f size = sf::Vector2f(64, 64))
        : IWidget(pos, size) {
    };
    void handleEvent(const sf::Event& event) override { }
    void setBounds(const sf::Vector2f pos, const sf::Vector2f size) override {
        pos_ = pos;
        size_ = size;
    }  //not completed!
    void setVisible(bool visible) override { visible_ = visible; }
    ~WidgetBase() override = default;
protected:
    bool visible_{true};
};
#endif //UNDEROCEAN_WIDGETBASE_H