//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_ILABEL_H
#define UNDEROCEAN_ILABEL_H
#include <SFML/System/Vector2.hpp>
#include "WidgetBase.h"
class ILabel : public WidgetBase{
public:
    ILabel(const sf::Vector2f pos, const sf::Vector2f size)
        : WidgetBase(pos, size) {
    };
    ILabel()
        : WidgetBase() {
    };
    virtual ~ILabel() = default;
};
#endif //UNDEROCEAN_ILABEL_H