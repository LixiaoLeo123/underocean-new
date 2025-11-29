//
// Created by 15201 on 11/23/2025.
//

#ifndef UNDEROCEAN_LEVEL0_H
#define UNDEROCEAN_LEVEL0_H
#include <queue>
#include <unordered_set>

#include "common/Types.h"
#include "common/net(depricate)/PacketChannel.h"
#include "common/net(depricate)/PacketReader.h"
#include "common/net(depricate)/PacketWriter.h"
#include "server/new/ILevel.h"
class Level0 : public ILevel {  //lobby, a place that handle out of specific level players
public:
    void update(float dt) override {};

    void onPlayerLeave(ENetPeer *player) override {};

    void onPlayerJoin(ENetPeer *player) override {};
};
#endif //UNDEROCEAN_LEVEL0_H