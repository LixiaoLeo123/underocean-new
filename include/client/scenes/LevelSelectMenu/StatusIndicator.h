//
// Created by 15201 on 11/26/2025.
//

#ifndef UNDEROCEAN_NETSTATUSINDICATOR_H
#define UNDEROCEAN_NETSTATUSINDICATOR_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>

#include "client/common/ResourceManager.h"

class StatusIndicator {
private:
    enum class State {
        CONNECTING,
        CONNECTED,
        PROCESSING   //when not connected and press level button, sprite flash 1 time
    };
public:
    explicit StatusIndicator(){
        sprite_.setTexture(ResourceManager::getTexture("images/others/loading.png"));
        sprite_.setTextureRect(sf::IntRect(0, 0, TEX_WIDTH * 9, TEX_HEIGHT));
        sprite_.setOrigin(TEX_WIDTH / 2.f, TEX_HEIGHT / 2.f);
    }
    void setFrameCount(int count) {  // 0 to 9
        currentFrameCount_ = count;
        sprite_.setTextureRect(sf::IntRect(0, TEX_HEIGHT * currentFrameCount_, TEX_WIDTH, TEX_HEIGHT));
        // std::cout << currentFrameCount_ << std::endl;
    }
    void nextFrame();
    void update(float dt) {
        animTime_ += dt;
        if (animTime_ > FRAME_TIME) {
            animTime_ = 0.0f;
            nextFrame();
        }
    }
    void render(sf::RenderWindow& window) const {
        window.draw(sprite_);
    }
    void adjustBound(const sf::View& newView);
    void setStateConnecting(){ state_ = State::CONNECTING; }
    void setStateConnected() { state_ = State::CONNECTED; }
    void setStateProcessing(){ state_ = State::PROCESSING; setFrameCount(5); frameDelta_ = -1; }
private:
    sf::Sprite sprite_{};
    float animTime_ = 0.0f;  //total, clear per frame
    int currentFrameCount_ { 9 };
    static constexpr float FRAME_TIME = 0.04f;
    static constexpr int TEX_WIDTH = 24;
    static constexpr int TEX_HEIGHT = 40;
    State state_ { State::CONNECTING };
    int frameDelta_ { 1 };   //dir
};

inline void StatusIndicator::nextFrame() {  //0-4, 5-9
    switch (state_) {
        case State::CONNECTING:
            if (currentFrameCount_ < 6 && frameDelta_ == -1) {
                frameDelta_ = 1;
            }
            else if (currentFrameCount_ > 8) {
                frameDelta_ = -1;
            }
            break;
        case State::CONNECTED:
            if (currentFrameCount_ < 1) {
                frameDelta_ = 1;
            }
            else if (currentFrameCount_ > 8 && frameDelta_ == 1) {
                frameDelta_ = -1;
            }
            break;
        case State::PROCESSING:
            if (currentFrameCount_ < 1) {
                frameDelta_ = 1;
                state_ = State::CONNECTING;
            }
            else if (currentFrameCount_ > 8){
                frameDelta_ = -1;
            }
            break;
    }
    setFrameCount(frameDelta_ + currentFrameCount_);
}

inline void StatusIndicator::adjustBound(const sf::View &newView) {
    sf::Vector2f viewCenter = newView.getCenter();
    sf::Vector2f viewSize = newView.getSize();
    float right = viewCenter.x + viewSize.x / 2.0f;
    float top = viewCenter.y - viewSize.y / 2.0f;
    float offsetX = viewSize.x * 0.05f;
    float offsetY = viewSize.y * 0.09f;   //
    sf::Vector2f targetPos(right - offsetX, top + offsetY);
    sprite_.setPosition(targetPos);
    float desiredWidth = viewSize.x * 0.05f; //area ratio
    float scale = desiredWidth / static_cast<float>(TEX_WIDTH);
    sprite_.setScale(scale, scale);
}
#endif //UNDEROCEAN_NETSTATUSINDICATOR_H
