//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_NETWORKCONTROLSYSTEM_H
#define UNDEROCEAN_NETWORKCONTROLSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/GameServer.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"

class NetworkControlSystem : public ISystem{
private:
    Signature signature_ {};
    Coordinator& coord_;  //to get entity and modify entity
    GameServer& server_;  //to get network information (PlayerData)
    UVector mapSize_;
public:
    explicit NetworkControlSystem(Coordinator& coordinator, GameServer& server, UVector mapSize)  //mapSize to trans from netPos to realPos
        :coord_(coordinator), server_(server), mapSize_(mapSize){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<NetworkPeer>()), true);
        coord_.registerSystem(signature_);
    }
    void update(float dt) override {
        auto& networkEntities = coord_.getEntitiesWith(signature_);
        for (Entity e : networkEntities) {
            auto& trans = coord_.getComponent<Transform>(e);
            ENetPeer* peer = coord_.getComponent<NetworkPeer>(e).peer;
            float worldX = mapSize_.x * static_cast<float>(server_.playerList_[peer].netX) / 65535.f;
            float worldY = mapSize_.y * static_cast<float>(server_.playerList_[peer].netY) / 65535.f;
            trans.x = worldX;  //update pos
            trans.y = worldY;
        }
    }
};
#endif //UNDEROCEAN_NETWORKCONTROLSYSTEM_H