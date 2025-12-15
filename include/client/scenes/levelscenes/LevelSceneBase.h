//
// Created by 15201 on 12/6/2025.
//

#ifndef UNDEROCEAN_LEVELSCENEBASE_H
#define UNDEROCEAN_LEVELSCENEBASE_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "PlayerStatus.h"
#include "SkillBar.h"
#include "client/common/ChatBox.h"
#include "client/common/InputManager.h"
#include "client/common/IScene.h"
#include "client/common/NetworkEntity.h"
#include "client/common/PlayerEntity.h"
#include "common/net(depricate)/PacketReader.h"
#include "common/net(depricate)/PacketWriter.h"
#include "common/network/ClientNetworkDriver.h"
#include "server/new/component/Components.h"


class LevelSceneBase : public IScene{
public:
    explicit LevelSceneBase(const std::shared_ptr<ClientNetworkDriver>& driver,
        ClientCommonPlayerAttributes& playerAttributes, const std::shared_ptr<ChatBox>& chatBox);
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event &event) override;
    void resetViewSize(unsigned windowWidth, unsigned windowHeight);
    void correctView();  //keep view inside map size
    void update(float dt) override;
    [[nodiscard]] unsigned getCurrentTick() const { return currentTick_; }
    virtual UVector getMapSize() = 0;
    virtual std::uint16_t ltonX(float x) = 0;  //local to net x
    virtual float ntolX(std::uint16_t x) = 0;
    virtual std::uint16_t ltonY(float y) = 0;  //local to net y
    virtual float ntolY(std::uint16_t y) = 0;
protected:
    sf::Sprite background_;
    sf::View view_ {};
    bool viewInit_ { false };  //not init
    std::shared_ptr<ChatBox> chatBox_;
    ClientCommonPlayerAttributes& playerAttributes_;
    void handleEntityStaticData();
    void handleEntityLeave();
    void handleEntityDynamic();
    void handlePlayerStateUpdate();  //fp, hp
    void handleEntitySizeChange();
    void handlePlayerAttributesUpdate();  //max hp, max fp, max vel, max acc
    void handleMessagePacket();
private:
    constexpr static float VIEW_WIDTH = 800.f;
    constexpr static float VIEW_HEIGHT = 450.f;
    float accDisRatio_ { -1.f };
    PlayerStatus playerStatus_ {};  //HP and FP indicator
protected:
    std::unordered_map<Entity, NetworkEntity> entities_;
    PlayerEntity player;
    std::shared_ptr<ClientNetworkDriver> driver_;  //given by LevelSelectMenu
    PacketWriter writer_;  //reuse
    SkillBar skillBar_;
    unsigned currentTick_ { 0 };
};
inline void LevelSceneBase::resetViewSize(unsigned windowWidth, unsigned windowHeight) {
    float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    constexpr static float viewRatio = VIEW_WIDTH / VIEW_HEIGHT;
    if (windowRatio > viewRatio) {
        float newHeight = VIEW_WIDTH / windowRatio;
        view_.setSize(VIEW_WIDTH, newHeight);
    } else {
        float newWidth = VIEW_HEIGHT * windowRatio;
        view_.setSize(newWidth, VIEW_HEIGHT);
    }
}
inline void LevelSceneBase::handleEvent(const sf::Event &event) {
    // if event is window resize, adjust sf::View, to maintain its ratio same as the window
    // while keeping inside VIEW_WIDTH and VIEW_HEIGHT with max size
    if (event.type == sf::Event::Resized) {
        resetViewSize(event.size.width, event.size.height);
        correctView();
        playerStatus_.onWindowSizeChange(event.size.width, event.size.height);
    }
    chatBox_->handleEvent(event);
    skillBar_.handleEvent(event);
}
inline void LevelSceneBase::correctView() {
    if (view_.getCenter().x < view_.getSize().x / 2)
        view_.setCenter(view_.getSize().x / 2, view_.getCenter().y);
    else if (view_.getCenter().x > getMapSize().x - view_.getSize().x / 2)
        view_.setCenter(getMapSize().x - view_.getSize().x / 2, view_.getCenter().y);
    if (view_.getCenter().y < view_.getSize().y / 2)
        view_.setCenter(view_.getCenter().x, view_.getSize().y / 2);
    else if (view_.getCenter().y > getMapSize().y - view_.getSize().y / 2)
        view_.setCenter(view_.getCenter().x, getMapSize().y - view_.getSize().y / 2);
}
#endif //UNDEROCEAN_LEVELSCENEBASE_H
