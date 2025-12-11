//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_NETWORKSYNCSYSTEM_H
#define UNDEROCEAN_NETWORKSYNCSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"

#include "common/utils/DBitset.h"
#include "server/new/EventBus.h"
#include "server/new/LevelBase.h"

class LevelBase;
class GameServer;

class NetworkSyncSystem : public ISystem {
private:
    Signature signature_ {};
    Signature aoiSignature_ {}; //for entities in aoi which need to sync
    Coordinator& coord_;
    GameServer& server_;  //use to send data
    LevelBase& level_;   //use to get map size
    DBitset sizeChangedBits_ {};  //entities that size changed this frame
    DBitset hpChangedBits_ {};  //entities that HP changed this frame
    struct PeerAOI {
        DBitset current;
        DBitset last;
        DBitset enterBits;  //for cache
        DBitset leaveBits;
        std::vector<Entity> enterList;   //buffer that can be used multiple times
        std::vector<Entity> leaveList;
        std::vector<Entity> dynamicList;  //all current entities
        std::uint16_t lastNetHP {0u};
        std::uint16_t lastNetFP {0u};
    };
    std::unordered_map<ENetPeer*, PeerAOI> peerAOIs;
    static void extractIDs(const DBitset& bits, std::vector<Entity>& out) {
        const uint64_t* words = bits.data();
        out.clear();
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            uint64_t w = words[i];
            if (w == 0) continue;
            size_t base = i * 64;
            while (w != 0) {
                int tz = __builtin_ctzll(w);  //trailing zeros
                out.push_back(static_cast<Entity>(base + tz));
                w &= w - 1;   //clear lowest 1
            }
        }
    }
    void onEntitySizeChange(const EntitySizeChangeEvent& event);
    void onEntityHPChange(const EntityHPChangeEvent& event);
    void onPlayerLeave(const PlayerLeaveEvent& event);
    void onPlayerJoin(const PlayerJoinEvent& event);
public:
    explicit NetworkSyncSystem(Coordinator &coordinator, GameServer &server, LevelBase &level, EventBus &eventbus)
        : coord_(coordinator), server_(server), level_(level) {
        {
            signature_.set(Coordinator::getComponentTypeID<NetworkPeer>(), true);
            signature_.set(Coordinator::getComponentTypeID<Transform>(), true);
            coord_.registerSystem(signature_);
        }
        eventbus.subscribe<EntitySizeChangeEvent>([this](const EntitySizeChangeEvent& event) {
            this->onEntitySizeChange(event);
        });
        eventbus.subscribe<EntityHPChangeEvent>([this](const EntityHPChangeEvent& event) {
            this->onEntityHPChange(event);
        });
        eventbus.subscribe<PlayerLeaveEvent>([this](const PlayerLeaveEvent& event) {
            this->onPlayerLeave(event);
        });
        eventbus.subscribe<PlayerJoinEvent>([this](const PlayerJoinEvent& event) {
            this->onPlayerJoin(event);
        });
        {
            aoiSignature_.set(Coordinator::getComponentTypeID<EntityType>(), true);
            aoiSignature_.set(Coordinator::getComponentTypeID<Transform>(), true);
            aoiSignature_.set(Coordinator::getComponentTypeID<Size>(), true);
            aoiSignature_.set(Coordinator::getComponentTypeID<NetSyncComp>(), true);
            coord_.registerSystem(aoiSignature_);
        }
    }
    void update(float dt) override;
};
inline void NetworkSyncSystem::onEntitySizeChange(const EntitySizeChangeEvent &event) {
    sizeChangedBits_.set(event.entity);
}
inline void NetworkSyncSystem::onEntityHPChange(const EntityHPChangeEvent &event) {
    hpChangedBits_.set(event.entity);
}
inline void NetworkSyncSystem::onPlayerLeave(const PlayerLeaveEvent& event) {
    ENetPeer* peer = event.peer;
    peerAOIs.erase(peer);
}
inline void NetworkSyncSystem::onPlayerJoin(const PlayerJoinEvent& event) {
    peerAOIs.try_emplace(event.playerData.peer, PeerAOI{});
    peerAOIs[event.playerData.peer].lastNetHP = ltonHP16(event.playerData.initHP);
    peerAOIs[event.playerData.peer].lastNetFP = ltonFP(event.playerData.initFP);
}
#endif //UNDEROCEAN_NETWORKSYNCSYSTEM_H
