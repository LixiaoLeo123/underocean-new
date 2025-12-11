//
// Created by 15201 on 11/18/2025.
//
#include "../include/client/scenes/NameScene/NameScene.h"
#include "client/common/ResourceManager.h"
#include "../include/client/scenes/startmenu/StartMenu.h"
#include "client/ui/widgets/TextButton.h"
#include "client/ui/widgets/labels/TextLabel.h"
#include "server/core(deprecate)/GameData.h"

NameScene::NameScene(const std::shared_ptr<SmoothTextLabel>& title)
    :LazyPanelScene(WIDTH, HEIGHT), title_(title) {
    sf::Font& font = ResourceManager::getFont("fonts/font4.ttf");
    {
        sf::Text text("Name the fish", font, 30);  //label
        sf::FloatRect bounds = text.getLocalBounds();
        std::shared_ptr<TextLabel> label = std::make_shared<TextLabel>(std::move(text));
        sf::Vector2f labelSize = label->calculateSizeByWidth(400);
        label->setBounds(sf::Vector2f((WIDTH - labelSize.x) / 2.f, 200), labelSize);
        add(label);
    }
    for (int j = 0; j < 6; ++j) {   //higher case
        for (int i = 0; i < 5; ++i) {
            if (j == 5 && i > 0) break;   //A to Z, sum 26
            sf::Vector2f pos((WIDTH * (1 + i)) / 12, (HEIGHT * (4 + j)) / 13);
            sf::Text characterText(std::string(1, static_cast<char>('A' + (j * 5 + i))), font, 12);
            std::shared_ptr<TextButton> button = std::make_shared<TextButton>(pos, WIDTH / 30, std::move(characterText),true);
            button->setColor(sf::Color::White, sf::Color::Yellow, sf::Color::Red);
            button->setOnClick([this, j, i]() {
                this->appendId(static_cast<char>('A' + (j * 5 + i)));
            });
            add(button);
        }
    }
    for (int j = 0; j < 6; ++j) {  //lower case
        for (int i = 0; i < 5; ++i) {
            if (j == 5 && i > 0) break;   //a to z, sum 26
            sf::Vector2f pos((WIDTH * (7 + i)) / 12, (HEIGHT * (4 + j)) / 13);
            sf::Text characterText(std::string(1, static_cast<char>('a' + (j * 5 + i))), font, 60);
            std::shared_ptr<TextButton> button = std::make_shared<TextButton>(pos, std::move(characterText),true);
            button->setColor(sf::Color::White, sf::Color::Yellow, sf::Color::Red);
            button->setOnClick([this, j, i]() {
                this->appendId(static_cast<char>('a' + (j * 5 + i)));
            });
            add(button);
        }
    }
    {
        sf::Vector2f pos(WIDTH * 0.13f, HEIGHT * 0.93f);  //backspace
        sf::Text characterText("backspace", font, 12);
        std::shared_ptr<TextButton> button = std::make_shared<TextButton>(pos, WIDTH / 25, std::move(characterText),false);
        button->setColor(sf::Color::White, sf::Color::Yellow, sf::Color::Red);
        button->setOnClick([this]() {
            this->backspaceId();
        });
        add(button);
    }
    {
        sf::Vector2f pos(WIDTH * 0.95f, HEIGHT * 0.92f);  //confirm
        sf::Text characterText("ok", font, 12);
        okButton_ = std::make_shared<TextButton>(pos, WIDTH / 28, std::move(characterText),false);
        okButton_->setColor(sf::Color(200,200,200),sf::Color(200,200,200),sf::Color(200,200,200));
        okButton_->setOnClick([this]() {
            this->tryConfirm();
        });
        add(okButton_);
    }
    {
        sf::Vector2f pos(WIDTH * 0.4f, HEIGHT * 0.8f);  //ID itself
        sf::Text idEmptyText("S", font, 24);
        idEmptyText.setFillColor(sf::Color::White);
        cachedId_ = std::make_shared<PlayerIdLabel>(std::move(idEmptyText));
        cachedId_->setBounds(pos, sf::Vector2f(40,40));
        cachedId_->backspace();  //default empty
        add(cachedId_);
    }
}

void NameScene::render(sf::RenderWindow &window) {
    switch (state_) {
        case Animation:
            window.setView(view_);
        case Entering:
            LazyPanelScene::render(window);
            renderPanel(window);
            break;
    }
}

void NameScene::handleEvent(const sf::Event &event) {
    switch (state_) {
        case Entering:
            LazyPanelScene::handleEvent(event);
            getPanel().handleEvent(event);
            break;
        case Animation:
            if (event.type == sf::Event::Resized) {
                float windowWidth = static_cast<float>(event.size.width);
                float windowHeight = static_cast<float>(event.size.height);
                float aspect = windowWidth / windowHeight;
                float viewWidth = std::sqrt(viewArea_ * aspect);
                float viewHeight = viewWidth / aspect;
                view_.setSize(viewWidth, viewHeight);
            }
            break;
    }
}

void NameScene::tryConfirm() {
    if (cachedId_->isEmpty()) return;
    state_ = Animation;
    viewArea_ = getPanel().getSize().x * getPanel().getSize().y;
    view_.setSize(getPanel().getSize());
    view_.setCenter(getPanel().getCenter());
    viewMovingDirection_ = (cachedId_->getPos() + sf::Vector2f(55, 10)) - getPanel().getCenter();
    cachedId_->setShakeState(true);
    clock_.restart();
}

void NameScene::update(float dt) {
    switch (state_) {
        case Entering:
            title_->updateTotal(dt);
            break;
        case Animation: {
            if (clock_.getElapsedTime().asSeconds() > ANIM_TIME) {
                state_ = Ready;
                break;
            }
            static constexpr float viewAreaDecreaseRate = 675000.f;
            static constexpr float viewCenterMovingRate = 0.37f;
            static constexpr float viewRotatingRate = 2.5f;
            viewArea_ -= viewAreaDecreaseRate * dt;
            sf::Vector2f size = view_.getSize();
            float currentArea = size.x * size.y;
            if (currentArea == 0.0f) return;
            float ratio = size.x / size.y;
            float newHeight = std::sqrt(viewArea_ / ratio);
            float newWidth = ratio * newHeight;
            view_.setSize(newWidth, newHeight);
            view_.move(viewMovingDirection_ * viewCenterMovingRate * dt);
            view_.rotate(viewRotatingRate * dt);
            cachedId_->addShakeIntensity(dt * 0.3f);
            break;
        }
        case Ready: {
            strncpy(GameData::playerId, cachedId_->getStr().c_str(), sizeof(GameData::playerId) - 1);
            GameData::playerId[sizeof(GameData::playerId) - 1] = '\0';
            GameData::saveSettings();  //save id
            title_->setOldView(view_);
            SceneSwitchRequest request = {
                SceneSwitchRequest::Replace,
                std::make_unique<StartMenu>(title_, false),
                0,
                0    //means no title anim
            };
            onRequestSwitch_(request);
        }
        break;
    }
}
