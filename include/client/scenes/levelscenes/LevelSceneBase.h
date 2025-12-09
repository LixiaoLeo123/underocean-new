//
// Created by 15201 on 12/6/2025.
//

#ifndef UNDEROCEAN_LEVELSCENEBASE_H
#define UNDEROCEAN_LEVELSCENEBASE_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

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
    explicit LevelSceneBase(const std::shared_ptr<ClientNetworkDriver>& driver)
        :driver_(driver) {
        player.setType(static_cast<EntityTypeID>(GameData::playerType));
        player.setSize(GameData::playerSize);
        // get max Acc when mouse is at the left or right side of the view
        accDisRatio_ = player.getMaxAcceleration() * 2 / VIEW_WIDTH;
        // send level change packet
    };
    void render(sf::RenderWindow& window) override {
        if (!viewInit_) {
            resetViewSize(window.getSize().x, window.getSize().y);
            correctView();
            viewInit_ = true;
        }
        window.setView(view_);
        window.draw(background_);
        for (auto& pair : entities_) {
            pair.second.render(window);
        }
        player.render(window);
    }
    void handleEvent(const sf::Event &event) override {
        // if event is window resize, adjust sf::View, to maintain its ratio same as the window
        // while keeping inside VIEW_WIDTH and VIEW_HEIGHT with max size
        if (event.type == sf::Event::Resized) {
            resetViewSize(event.size.width, event.size.height);
            correctView();
        }
    }
    void resetViewSize(unsigned windowWidth, unsigned windowHeight) {
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
    void correctView();  //keep view inside map size
    void update(float dt) override;
    void handleEntityStaticData();
    void handleEntityLeave();
    void handleEntityDynamic();
    virtual UVector getMapSize() = 0;
    virtual std::uint16_t ltonX(float x) = 0;  //local to net x
    virtual float ntolX(std::uint16_t x) = 0;
    virtual std::uint16_t ltonY(float y) = 0;  //local to net y
    virtual float ntolY(std::uint16_t y) = 0;
protected:
    sf::Sprite background_;
private:
    sf::View view_ {};
    bool viewInit_ { false };  //not init
    constexpr static float VIEW_WIDTH = 90.f;
    constexpr static float VIEW_HEIGHT = 50.f;
    float accDisRatio_ { -1.f };
protected:
    std::unordered_map<Entity, NetworkEntity> entities_;
    PlayerEntity player;
    std::shared_ptr<ClientNetworkDriver> driver_;  //given by LevelSelectMenu
    PacketWriter writer_;  //reuse
};
inline void LevelSceneBase::update(float dt) {
    driver_->pollPacket();
    handleEntityStaticData();
    handleEntityLeave();
    handleEntityDynamic();
    for (auto& pair : entities_) {
        pair.second.update(dt);
    }
    sf::Vector2f playerRawAcc {};  // without clamp
    if (InputManager::getInstance().mousePressedLeft) {
        sf::Vector2f mouseWorld = InputManager::getInstance().mousePosWorld;
        sf::Vector2f dir = mouseWorld - player.getPosition();
        playerRawAcc = accDisRatio_ * dir;
    }
    else {
        playerRawAcc = -10.f * dt * player.getVelocity();
    }
    player.update(dt, playerRawAcc);
    //move view toward player position
    view_.move(GameData::CAMERA_ALPHA * (player.getPosition() - view_.getCenter()));
    correctView();
    //send player pos packet to server
    writer_.writeInt16(ltonX(player.getPosition().x))
        .writeInt16(ltonY(player.getPosition().y));
    driver_->send(writer_.takePacket(), 0, ServerTypes::PacketType::PKT_TRANSFORM, false);
    writer_.clearBuffer();
}
inline void LevelSceneBase::handleEntityStaticData() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_STATIC_DATA)) {
        if (packet->size() % 8 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            auto type = static_cast<EntityTypeID>(reader.nextUInt8());
            std::uint8_t netSize = reader.nextUInt8();
            std::uint16_t netX = reader.nextUInt16();
            std::uint16_t netY = reader.nextUInt16();
            auto& entity = entities_[entityId];   //will create if not exist, replace if exist
            entity.setType(type);
            entity.setSize(ntolSize(netSize));
            entity.setNetworkState({ntolX(netX), ntolY(netY)});
        }
    }
}
inline void LevelSceneBase::handleEntityLeave() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_LEAVE)) {
        if (packet->size() % 2 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            entities_.erase(entityId);
        }
    }
}
inline void LevelSceneBase::handleEntityDynamic() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_DYNAMIC_DATA)) {
        if (packet->size() % 6 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            std::uint16_t netX = reader.nextUInt16();
            std::uint16_t netY = reader.nextUInt16();
            auto it = entities_.find(entityId);
            if (it != entities_.end()) {
                it->second.setNetworkState({ntolX(netX), ntolY(netY)});
            }
        }
    }
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
