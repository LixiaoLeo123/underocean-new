//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_NAMESCENE_H
#define UNDEROCEAN_NAMESCENE_H
#include "PlayerIdLabel.h"
#include "../LazyPanelScene.h"
#include "client/common/IScene.h"
#include "client/ui/widgets/TextButton.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"


class NameScene : public LazyPanelScene {
private:
    enum State {
        Entering,
        Animation,
        Ready  //to start scene
    };
    State state_{Entering};
public:
    explicit NameScene(const std::shared_ptr<SmoothTextLabel>& title);
    void render(sf::RenderWindow &window) override;
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void whenViewInit(const sf::View& view) override {
        sf::Vector2f titleSize = title_->calculateSizeByWidth(WIDTH / 3.f);
        title_->convertTo(view);                //to cancel the pos bias due to the change of view
        title_->setBounds(sf::Vector2f((WIDTH - titleSize.x) / 2.f, HEIGHT / 40.f), titleSize);
        add(title_);
    }
    void backspaceId() {
        cachedId_->backspace();
        if (cachedId_->isEmpty()) {
            okButton_->setColor(sf::Color(200,200,200),
                sf::Color(200,200,200),sf::Color(200,200,200));  //set grey
        }
    }
    void appendId(char c) {
        cachedId_->append(c);
        if (!cachedId_->isEmpty()) {
            okButton_->setColor(sf::Color::White, sf::Color::Yellow, sf::Color::Red);  //set responsive
        }
    }
    void tryConfirm();    //call by ok button, but need check str len
private:
    std::shared_ptr<SmoothTextLabel> title_;
    std::shared_ptr<PlayerIdLabel> cachedId_;
    std::shared_ptr<TextButton> okButton_;  //to set the color
    sf::View view_;    //only useful when state_ == Animation
    float viewArea_ = -1.f;  //same
    sf::Vector2f viewMovingDirection_;  //same
    sf::Clock clock_;  //for anim
    inline static float WIDTH = 1920.f;
    inline static float HEIGHT = 1080.f;
    constexpr static float ANIM_TIME = 5.f;
};
#endif //UNDEROCEAN_NAMESCENE_H