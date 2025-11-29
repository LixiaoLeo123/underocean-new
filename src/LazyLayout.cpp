//
// Created by 15201 on 11/18/2025.
//

#include "client/ui/layouts/LazyLayout.h"

#include "client/ui/IWidget.h"

void LazyLayout::handleLayout(std::vector<std::shared_ptr<IWidget>>& widgets, const sf::Vector2f parentPos, const sf::Vector2f parentSize) {
    float originRatio = originSize_.x / originSize_.y;
    float parentRatio = parentSize.x / parentSize.y;
    auto [newOriginSize, newOrigin] = [&]() -> std::pair<sf::Vector2f, sf::Vector2f> {
        if (originRatio > parentRatio) {
            sf::Vector2f size = originSize_ * parentSize.x / originSize_.x;
            sf::Vector2f pos(parentPos.x, parentPos.y + (parentSize.y - size.y) / 2.f);
            return { size, pos };
        }
        else {
            sf::Vector2f size = originSize_ * parentSize.y / originSize_.y;
            sf::Vector2f pos(parentPos.x + (parentSize.x - size.x) / 2.f, parentPos.y);
            return { size, pos };
        }
    }();
    float sizeRatio = newOriginSize.x / originSize_.x;
    for (auto& widget : widgets) {
        sf::Vector2f newPos = newOrigin + (widget->pos_ - origin_) * sizeRatio;
        sf::Vector2f newSize = widget->size_ * sizeRatio;
        widget->setBounds(newPos, newSize);
    }
    origin_ = newOrigin;
    originSize_ = newOriginSize;
}
