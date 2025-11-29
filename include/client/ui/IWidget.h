//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_IWIDGET_H
#define UNDEROCEAN_IWIDGET_H
#include "IRenderable.h"
namespace sf {
    class Event;
}
class IWidget : public IRenderable{
protected:
    sf::Vector2f pos_;
    sf::Vector2f size_;
public:
    IWidget(const sf::Vector2f pos, const sf::Vector2f size)
        :pos_(pos), size_(size) {
    };
    [[nodiscard]] sf::Vector2f getPos() const {    //all pos are world-based
        return pos_;
    };
    [[nodiscard]] virtual sf::Vector2f getSize() const {
        return size_;
    }
    virtual void setBounds(sf::Vector2f pos, sf::Vector2f size) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void setVisible(bool visible) = 0;
    ~IWidget() override = default;
    friend class GridLayout;
    friend class LazyLayout;
};
#endif //UNDEROCEAN_IWIDGET_H