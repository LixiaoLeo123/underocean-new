//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_NETWORKSYNCSYSTEM_H
#define UNDEROCEAN_NETWORKSYNCSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/core/GameData.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include "server/new/resources/GridResource.h"
#include <windows.h>

#include "common/utils/DBitset.h"

class GameServer;

class NetworkSyncSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord_;
    GameServer& server_;  //use to send data
    struct PeerAOI {  //
        DBitset current;
        DBitset last;
        DBitset enterBits;  //for cache
        DBitset leaveBits;
        std::vector<Entity> enterList;   //buffer that can be used multiple times
        std::vector<Entity> leaveList;
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
public:
    explicit NetworkSyncSystem(Coordinator& coordinator, GameServer& server)
        :coord_(coordinator), server_(server) {
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<NetworkPeer>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<EntityType>()), true);
        coord_.registerSystem(signature_);
    }
    void update(float dt) override;
};
#endif //UNDEROCEAN_NETWORKSYNCSYSTEM_H
