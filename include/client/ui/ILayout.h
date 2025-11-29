//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_ILAYOUT_H
#define UNDEROCEAN_ILAYOUT_H
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>
class IWidget;
class ILayout {
public:
    virtual void handleLayout(std::vector<std::shared_ptr<IWidget>>& widgets,
        sf::Vector2f parentPos, sf::Vector2f parentSize) = 0;
    virtual ~ILayout() = default;
};
#endif //UNDEROCEAN_ILAYOUT_H