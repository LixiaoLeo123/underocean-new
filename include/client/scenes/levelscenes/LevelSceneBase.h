//
// Created by 15201 on 12/6/2025.
//

#ifndef UNDEROCEAN_LEVELSCENEBASE_H
#define UNDEROCEAN_LEVELSCENEBASE_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "client/common/IScene.h"
#include "client/common/NetworkEntity.h"
#include "common/net(depricate)/PacketReader.h"
#include "common/network/ClientNetworkDriver.h"
#include "server/new/component/Components.h"

class LevelSceneBase : public IScene{
public:
    LevelSceneBase(const std::shared_ptr<ClientNetworkDriver>& driver)
        :driver_(driver){};
    void render(sf::RenderWindow& window) override {
        window.draw(background_);
    }
    void handleEvent(const sf::Event &event) override {

    }
    void update(float dt) override;
    void handleEntityStaticData();
    void handleEntityLeave();
    void handleEntityDynamic();
    virtual UVector getMapSize() = 0;
    virtual std::uint16_t ltonX(float x) = 0;  //local to net x
    virtual float ntolX(std::uint16_t x) = 0;
    virtual std::uint16_t ltonY(float y) = 0;  //local to net y
    virtual float ntolY(std::uint16_t y) = 0;
private:
    sf::Sprite background_;
protected:
    std::unordered_map<Entity, NetworkEntity> entities_;
    NetworkEntity player;
    std::shared_ptr<ClientNetworkDriver> driver_;  //given by LevelSelectMenu
};
inline void LevelSceneBase::update(float dt) {
    driver_->pollPacket();
    handleEntityStaticData();
    handleEntityLeave();
    handleEntityDynamic();
}
inline void LevelSceneBase::handleEntityStaticData() {
    while (driver_->hasPacket(PacketType::PKT_ENTITY_STATIC_DATA)) {
        std::unique_ptr<Packet> packet(driver_->popPacket(PacketType::PKT_ENTITY_STATIC_DATA));
        if (packet->size() % 8 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));

    }
}
inline void LevelSceneBase::handleEntityLeave() {

}
inline void LevelSceneBase::handleEntityDynamic() {

}
#endif //UNDEROCEAN_LEVELSCENEBASE_H
