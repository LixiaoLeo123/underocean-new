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
    LevelBase& level_;    //to convert net pos and local pos
    std::unordered_map<ENetPeer*, UVector> networkEntityPrevTransform_ {};  //for velocity calculating
public:
    explicit NetworkControlSystem(Coordinator& coordinator, GameServer& server, LevelBase& level, EventBus& eventBus)  //mapSize to trans from netPos to realPos
        :coord_(coordinator), server_(server), level_(level){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<NetworkPeer>()), true);
        coord_.registerSystem(signature_);
        eventBus.subscribe<PlayerLeaveEvent>([this](const PlayerLeaveEvent& event) {
            this->onPlayerLeave(event);
        });
    }
    void update(float dt) override {
        auto& networkEntities = coord_.getEntitiesWith(signature_);
        for (Entity e : networkEntities) {
            auto& trans = coord_.getComponent<Transform>(e);
            ENetPeer* peer = coord_.getComponent<NetworkPeer>(e).peer;
            float worldX = level_.ntolX(server_.playerList_[peer].netX);
            float worldY = level_.ntolY(server_.playerList_[peer].netY);
            trans.x = worldX;  //update pos
            trans.y = worldY;
            auto prevIt = networkEntityPrevTransform_.find(peer);
            if (prevIt != networkEntityPrevTransform_.end()) {
                coord_.getComponent<Velocity>(e) = ((UVector)trans - prevIt->second) / dt;  //update velocity
            }
            networkEntityPrevTransform_[peer] = trans;  //update prev pos
        }
    }
    void onPlayerLeave(const PlayerLeaveEvent& event) {
        ENetPeer* peer = event.peer;
        networkEntityPrevTransform_.erase(peer);
    }
};
#endif //UNDEROCEAN_NETWORKCONTROLSYSTEM_H