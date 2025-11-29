//
// Created by 15201 on 11/18/2025.
//

#include "client/scenes/RootScene.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "client/common/ResourceManager.h"
#include "../include/client/scenes/flashscreen/FlashScreen.h"

sf::Event RootScene::dispatchEvent(sf::RenderWindow& window, const sf::Event& rawEvent) {  //convert event pos from pixel to world
    sf::Event worldEvent = rawEvent;
    if (worldEvent.type == sf::Event::MouseMoved) {
        auto worldPos = window.mapPixelToCoords({
            worldEvent.mouseMove.x,
            worldEvent.mouseMove.y
        });
        worldEvent.mouseMove.x = static_cast<int>(worldPos.x);
        worldEvent.mouseMove.y = static_cast<int>(worldPos.y);
    } else if (worldEvent.type == sf::Event::MouseButtonPressed ||
               worldEvent.type == sf::Event::MouseButtonReleased) {
        auto worldPos = window.mapPixelToCoords({
            worldEvent.mouseButton.x,
            worldEvent.mouseButton.y
        });
        worldEvent.mouseButton.x = static_cast<int>(worldPos.x);
        worldEvent.mouseButton.y = static_cast<int>(worldPos.y);
               }
    return worldEvent;
}

RootScene::RootScene(sf::RenderWindow& window)
    :font0_(ResourceManager::getFont("fonts/font0.otf")),
    title_(std::make_shared<SmoothTextLabel>(sf::Text("UNDEROCEAN", font0_, 30), 0.1f)) {
    sf::Vector2f titleSize = sf::Vector2f(window.getSize().x / 1.7f, window.getSize().x / 1.7f / title_->getRatio());
    title_->setBoundsWithoutAnim(sf::Vector2f((window.getSize().x - titleSize.x) / 2.f, (window.getSize().y - titleSize.y) / 3.4f),
        titleSize);
    //title_->setFillColor(sf::Color::White);
    //title_->setOutlineColor(sf::Color::White, 0);
    title_->setVisible(false);
    pushScene<FlashScreen>(title_, window);
}
