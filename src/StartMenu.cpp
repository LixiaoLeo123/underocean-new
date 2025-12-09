//
// Created by 15201 on 11/18/2025.
//

#include "../include/client/scenes/startmenu/StartMenu.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <utility>
#include "client/common/ResourceManager.h"
#include "client/scenes/LevelSelectMenu/LevelSelectMenu.h"
#include "client/ui/layouts/LazyLayout.h"
#include "client/ui/widgets/TextButton.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "server/core(deprecate)/GameData.h"

StartMenu::StartMenu(const std::shared_ptr<SmoothTextLabel>& title, bool titleSmooth)   //only logic init! originView for setBoundsWithoutAnim
    :font0_(ResourceManager::getFont("fonts/font0.otf")),
    font1_(ResourceManager::getFont("fonts/font4.ttf")),
    title_(title),
    shark_(ResourceManager::getTexture("images/backgrounds/bg1/shark.png"), sf::Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f), 4, 1, sf::FloatRect(WIDTH / 5.0f, HEIGHT / 5.0f, WIDTH * 0.6f, HEIGHT * 0.6f)),
    bubble0_(ResourceManager::getTexture("images/backgrounds/bg1/bubble0.png"), HEIGHT, -0.1f, 0.f, 10.f, 0.06f),
    bubble1_(ResourceManager::getTexture("images/backgrounds/bg1/bubble1.png"), HEIGHT, 0.15f, 1.f, 10.f, 0.06f),
    boat_(ResourceManager::getTexture("images/backgrounds/bg1/boat.png")),
    selectPanel_(sf::Vector2f(0,0), sf::Vector2f(WIDTH, HEIGHT),
        std::make_unique<LazyLayout>(sf::Vector2f(WIDTH, HEIGHT), sf::Vector2f(0, 0))){
    background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg1/bg1.png"));
    titleSmooth_ = titleSmooth;
    {
        sf::Vector2f pos(WIDTH / 2.f, HEIGHT * 0.52f);
        sf::Text characterText("Single Player", font1_, 24);
        std::shared_ptr<TextButton> singlePlayerB = std::make_shared<TextButton>(pos, WIDTH / 20, std::move(characterText),false);
        singlePlayerB->setColor(sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        singlePlayerB->setOnClick([this]() {
            this->onClickSinglePlayer();
        });
        selectPanel_.add(singlePlayerB);
    }
    {
        sf::Vector2f pos(WIDTH / 2.f, HEIGHT * 0.75f);
        sf::Text characterText("Multiple Player", font1_, 24);
        std::shared_ptr<TextButton> multiPlayerB = std::make_shared<TextButton>(pos, WIDTH / 20, std::move(characterText),false);
        multiPlayerB->setColor(sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        multiPlayerB->setOnClick([this]() {
            this->onClickMultiPlayer();
        });
        selectPanel_.add(multiPlayerB);
    }
    title_->setOutlineColor(sf::Color::Black, 1);
    title_->setVisible(true);
}

void StartMenu::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        unsigned int winWidth = event.size.width;
        unsigned int winHeight = event.size.height;
        resetViewArea(winWidth, winHeight);
        selectPanel_.setBounds(view_.getCenter() - view_.getSize() / 2.f, view_.getSize());
    }
    selectPanel_.handleEvent(event);
}

void StartMenu::render(sf::RenderWindow &window) {
    if (!viewInit_) {    //render area init
        sf::Vector2u winSize(window.getSize());
        resetViewArea(winSize.x, winSize.y);
        sf::Vector2f titleSize = title_->calculateSizeByWidth(WIDTH * 0.7f);
        if (titleSmooth_) {
            title_->convertTo(view_);                //to cancel the pos bias due to the change of view
            title_->setBounds(sf::Vector2f((WIDTH - titleSize.x) / 2.f, HEIGHT / 27.f), titleSize);
        }else {  //from namescene
            title_->setBoundsWithoutAnim(sf::Vector2f((WIDTH - titleSize.x) / 2.f, HEIGHT / 27.f), titleSize);
        }
        selectPanel_.add(title_);
        selectPanel_.setBounds(view_.getCenter() - view_.getSize() / 2.f, view_.getSize());
        window.setView(view_);
        viewInit_ = true;
    }
    if (viewDirty_) {
        viewDirty_ = false;
        window.setView(view_);
    }
    window.draw(background_);
    shark_.render(window);
    window.draw(boat_);
    bubble0_.render(window);
    bubble1_.render(window);
    selectPanel_.render(window);
}

void StartMenu::onClickSinglePlayer() {
    // MemoryClientChannel clientChannel;
    // std::unique_ptr<MemoryServerChannel> serverChannel = std::make_unique<MemoryServerChannel>();
    // GameServer<MemoryServerChannel> gameServer(std::move(serverChannel)); //server start!
    // title_->setOldView(view_);
    // SceneSwitchRequest request = {
    //     SceneSwitchRequest::Push,
    //     std::make_unique<LevelSelectMenu<MemoryClientChannel>>(title_, clientChannel),
    //     0,
    //     0    //means no title anim
    // };
    // onRequestSwitch_(request);
}

void StartMenu::onClickMultiPlayer() {
    title_->setOldView(view_);
    SceneSwitchRequest request = {
        SceneSwitchRequest::Push,
        std::make_unique<LevelSelectMenu>(title_, GameData::SERVER_IP, GameData::SERVER_PORT),
        0,
        0 //means no title anim
    };
    onRequestSwitch_(request);
}
