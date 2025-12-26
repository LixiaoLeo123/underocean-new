//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_STATE_H
#define UNDEROCEAN_STATE_H
#include <functional>
#include <memory>
#include <utility>

#include "common/net(depricate)/enet.h"

class ILevel {
public:
    ILevel()= default;
    virtual ~ILevel() = default;
    virtual void update(float dt) = 0;
    virtual void onPlayerLeave(PlayerData& playerData) = 0;
    virtual void onPlayerJoin(PlayerData& playerData) = 0;
    virtual void handleRequestRespawn(ENetPeer* peer) = 0;
    virtual int getLevel() = 0;
};
#endif //UNDEROCEAN_STATE_H