//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_PANEL_H
#define UNDEROCEAN_PANEL_H
#include "WidgetBase.h"
#include "client/ui/ILayout.h"
#include <SFML/Graphics/VertexArray.hpp>
class Panel : public WidgetBase{
private:
    void drawBorder(sf::RenderWindow& window) const {
        sf::VertexArray lines(sf::LineStrip, 5);
        lines[0].position = pos_;
        lines[1].position = sf::Vector2f(pos_.x + size_.x, pos_.y);
        lines[2].position = sf::Vector2f(pos_.x + size_.x, pos_.y + size_.y);
        lines[3].position = sf::Vector2f(pos_.x, pos_.y + size_.y);
        lines[4].position = pos_ + size_ / 2.f;
        for (std::size_t i = 0; i < lines.getVertexCount(); ++i)
            lines[i].color = sf::Color::White;
        window.draw(lines);
    }
protected:
    std::vector<std::shared_ptr<IWidget>> children_;
    std::unique_ptr<ILayout> layout_;
public:
    Panel(const sf::Vector2f pos, const sf::Vector2f size, std::unique_ptr<ILayout> layout)
    :WidgetBase(pos, size){
        layout_ = std::move(layout);
    }
    void render(sf::RenderWindow& window) override {
        for (auto& child : children_) {
            child->render(window);
        }
#ifndef NDEBUG
        drawBorder(window);
#endif
    }
    void handleEvent(const sf::Event& event) override { //not window resize, see below
        for (auto& child : children_) {
            child->handleEvent(event);
        }
    }
    void setBounds(const sf::Vector2f pos, const sf::Vector2f size) override {
        pos_ = pos;
        size_ = size;
        if (layout_)  //null then do nothing
            layout_->handleLayout(children_, pos, size);
    }
    void add(std::shared_ptr<IWidget> widget) {
        children_.push_back(widget);
    }
    template<typename T, typename... Args>
    void create(Args&&... args) {     //ownership totally inside the panel
        children_.push_back(std::make_shared<T>(std::forward<Args>(args)...));
    }
    [[nodiscard]] sf::Vector2f getCenter() const {
        return pos_ + size_ / 2.f;
    }
    ~Panel() override = default;
};
#endif //UNDEROCEAN_PANEL_H