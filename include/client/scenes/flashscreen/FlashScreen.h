//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_FLASHSCREEN_H
#define UNDEROCEAN_FLASHSCREEN_H
#include "LoadingIndicator.h"
#include "../NameScene/NameScene.h"
#include "client/common/IScene.h"
#include "client/common/KeyBindingManager.h"
#include "../startmenu/StartMenu.h"
#include "client/common/AudioManager.h"
#include "client/ui/layouts/LazyLayout.h"
#include "client/ui/widgets/Panel.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "server/core(deprecate)/GameData.h"

class SmoothTextLabel;

class FlashScreen final : public IScene {
public:
    FlashScreen(const std::shared_ptr<SmoothTextLabel> &title, const sf::RenderWindow& window)
        :title_(title),
        panel_(sf::Vector2f(), sf::Vector2f(window.getSize().x, window.getSize().y),
                std::make_unique<LazyLayout>(sf::Vector2f(window.getSize().x, window.getSize().y), sf::Vector2f())) {
        panel_.add(title_);
        sf::Text tmp("[ press z or enter to continue ]", ResourceManager::getFont("fonts/font4.ttf"), 20);
        tmp.setFillColor(sf::Color(200, 200, 200));
        skipTip_ = std::make_shared<TextLabel>(std::move(tmp));
        sf::Vector2f skipTipSize(window.getSize().x / 3.f, window.getSize().x / 3.f / skipTip_->getRatio());
        skipTip_->setBounds(sf::Vector2f((window.getSize().x - skipTipSize.x) / 2.f, window.getSize().y * 0.7f),
            skipTipSize);
        skipTip_->setVisible(false);
        loadingIndicator_ = std::make_shared<LoadingIndicator>();
        loadingIndicator_->setBounds(sf::Vector2f((window.getSize().x) / 2.f, window.getSize().y * 0.73f),
            skipTipSize * 0.2f);  //same as skip tip to keep smooth
        loadingIndicator_->setVisible(false);
        panel_.add(skipTip_);
        panel_.add(loadingIndicator_);
        view_.setCenter(panel_.getCenter());
        view_.setSize(panel_.getSize());
    }
    void render(sf::RenderWindow& window) override {
        if (viewDirty_) window.setView(view_);
        panel_.render(window);
    }
    void handleEvent(const sf::Event& event) override {
        if (event.type == sf::Event::Resized) {
            panel_.setBounds(sf::Vector2f(0, 0), sf::Vector2f(event.size.width, event.size.height));
            view_.setCenter(panel_.getCenter());
            view_.setSize(event.size.width, event.size.height);
            viewDirty_ = true;
        }
        if (state_ == Skip)
            if (KeyBindingManager::hasAction(KeyBindingManager::Interact, event)) {
                if (GameData::playerId[0] == '@') {  //set name
                    title_->setOldView(view_);
                    SceneSwitchRequest request = {
                        SceneSwitchRequest::Replace,
                        std::make_unique<NameScene>(title_),
                        0,
                        -1
                    };
                    onRequestSwitch_(request);
                }
                else {   //select mode
                    title_->setOldView(view_);
                    SceneSwitchRequest request = {
                        SceneSwitchRequest::Replace,
                        std::make_unique<StartMenu>(title_, true),
                        0,
                        -1
                    };
                    onRequestSwitch_(request);
                }
            }
    }
    void update(float dt) override {
        if (float time = clock_.getElapsedTime().asSeconds(); time > blackScreenDelay) {
            state_ = ShowTitle;
            title_->setVisible(true);
            if (!title0SoundPlayed_) {
                AudioManager::getInstance().playSound("audio/s_title.wav");
                title0SoundPlayed_ = true;
            }
            if (time > loadingDelay) {
                state_ = Loading;
                loadingIndicator_->setVisible(true);
                if (ResourceManager::hasPreloaded()) {
                    loadingIndicator_->setVisible(false);
                    state_ = Skip;
                    skipTip_->setVisible(true);
                    if (!title1SoundPlayed_) {
                        AudioManager::getInstance().playSound("audio/s_title.wav");
                        title1SoundPlayed_ = true;
                    }
                }
            }
        }
        title_->updateTotal(dt);
        if (state_ == Loading) loadingIndicator_->update(dt);
    }
    //bool shouldUpdateWhenNotActive() override { return true; }
private:
    enum State {
        BlackScreen,
        ShowTitle,
        Loading,
        Skip
    };
    std::shared_ptr<SmoothTextLabel> title_;
    std::shared_ptr<TextLabel> skipTip_;
    std::shared_ptr<LoadingIndicator> loadingIndicator_ {};
    sf::Clock clock_;
    sf::View view_;
    bool viewDirty_{false};
    Panel panel_;
    State state_{BlackScreen};
    bool title0SoundPlayed_{false};
    bool title1SoundPlayed_{false};
    static constexpr float blackScreenDelay = 1.5f;   //on start
    static constexpr float loadingDelay = 3.f;    //show loading anim
};
#endif //UNDEROCEAN_FLASHSCREEN_H