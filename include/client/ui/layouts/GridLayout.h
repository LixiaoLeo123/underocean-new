//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_GRIDLAYOUT_H
#define UNDEROCEAN_GRIDLAYOUT_H
#include <cassert>
#include "client/ui/ILayout.h"
#include "client/ui/IWidget.h"

class GridLayout : public ILayout{
protected:
    int width_;
    int height_;
public:
    GridLayout(int width, int height)
        :width_(width), height_(height) {
        assert(width > 0 && height > 0);
    };
    void handleLayout(std::vector<std::shared_ptr<IWidget>>& widgets,
        const sf::Vector2f parentPos, const sf::Vector2f parentSize) override {
        int avgWidth = parentSize.x / width_;
        int avgHeight = parentSize.x / height_;
        for (int i = 0; i < width_; ++i) {
            for (int j = 0; j < height_; ++j) {
                if ((i * width_ + j) >= widgets.size()) return;
                widgets[i * width_ + j]->setBounds(sf::Vector2f(parentPos.x + avgWidth * i, parentPos.y + avgHeight * j),
                    sf::Vector2f(avgWidth, avgHeight));
            }
        }
    }
};
#endif //UNDEROCEAN_GRIDLAYOUT_H