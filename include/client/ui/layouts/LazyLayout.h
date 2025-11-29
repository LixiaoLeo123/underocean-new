//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_LAZYLAYOUT_H
#define UNDEROCEAN_LAZYLAYOUT_H
#include "client/ui/ILayout.h"
class IWidget;
class LazyLayout : public ILayout {   //constant ratio
public:
    LazyLayout(sf::Vector2f originSize, sf::Vector2f origin)
        :originSize_(originSize), origin_(origin) {
    }
    void handleLayout(std::vector<std::shared_ptr<IWidget>>& widgets,
        sf::Vector2f parentPos, sf::Vector2f parentSize) override;
private:
    sf::Vector2f originSize_;     //the "actual" panel size with ui widgets
    sf::Vector2f origin_;   //base point, left up
};
#endif //UNDEROCEAN_LAZYLAYOUT_H