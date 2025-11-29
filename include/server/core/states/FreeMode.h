//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_FREEMODE_H
#define UNDEROCEAN_FREEMODE_H
#include <SFML/Graphics/Sprite.hpp>

#include "../../new/Level.h"
#include "server/game/OldEntity.h"
#include "server/game/FishFactory.h"


class FreeMode final : public Level {
public:
    explicit FreeMode(sf::RenderWindow& contextWindow);
    ~FreeMode() override = default;
    void handleInput(const sf::Event& event) override {
        Level::handleInput(event);  //resize view
    }
    void update(float dt) override;
    void draw() override{
        window.setView(view);
        window.draw(background);
        for (const auto& entity : pEntities)
            entity->draw(window);
    }
private:
    static constexpr int ROWS = 6;    //6, 30
    static constexpr int COLS = 30;
    static constexpr int WIDTH = 1728;
    static constexpr int HEIGHT = 324;
    sf::Sprite background;
    std::vector<std::unique_ptr<OldEntity>> pEntities;  //the zero one is player   the only ownership
    std::vector<std::vector<std::vector<OldEntity*>>> grid;
    FishFactory fishFactory;
    void generateFishFactory();
    void correctViewRange();   //in WIDTH * HEIGHT area
};
inline void FreeMode::correctViewRange() {
    if (view.getCenter().x < view.getSize().x / 2)
        view.setCenter(view.getSize().x / 2, view.getCenter().y);
    else if (view.getCenter().x > WIDTH - view.getSize().x / 2)
        view.setCenter(WIDTH - view.getSize().x / 2, view.getCenter().y);
    if (view.getCenter().y < view.getSize().y / 2)
        view.setCenter(view.getCenter().x, view.getSize().y / 2);
    else if (view.getCenter().y > HEIGHT - view.getSize().y / 2)
        view.setCenter(view.getCenter().x, HEIGHT - view.getSize().y / 2);
}
#endif //UNDEROCEAN_FREEMODE_H