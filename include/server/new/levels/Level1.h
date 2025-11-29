//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVEL1_H
#define UNDEROCEAN_LEVEL1_H
#include "server/new/Coordinator.h"
#include "server/new/ILevel.h"
#include "server/new/LevelBase.h"

class Level1 final : LevelBase {
public:
    Level1();;

    void update(float dt) override;

    void onPlayerLeave(ENetPeer *player) override;

    void onPlayerJoin(ENetPeer *player) override;
private:
    Coordinator coordinator;  //ecs
};
inline void Level1::update(float dt) {
    LevelBase::update(dt);
}
#endif //UNDEROCEAN_LEVEL1_H
