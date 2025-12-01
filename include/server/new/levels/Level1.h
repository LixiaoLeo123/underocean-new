//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVEL1_H
#define UNDEROCEAN_LEVEL1_H
#include "server/new/Coordinator.h"
#include "server/new/ILevel.h"
#include "server/new/LevelBase.h"
#include "server/new/resources/plots/PlotContext1.h"
#include "server/new/system/BoidsSystem.h"

class Level1 final : LevelBase {
public:
    Level1();

    void update(float dt) override;

    void onPlayerLeave(ENetPeer *player) override;

    void onPlayerJoin(ENetPeer *player) override;
protected:
    void customInitialize() override {
        systems_.emplace_back(std::make_unique<BoidsSystem>(coordinator_));
    };
};
inline Level1::Level1() {
    coordinator_.emplaceContext<PlotContext1>();

}
inline void Level1::onPlayerLeave(ENetPeer *player) {
}
inline void Level1::onPlayerJoin(ENetPeer *player) {
}
inline void Level1::update(float dt) {
    LevelBase::update(dt);
}
#endif //UNDEROCEAN_LEVEL1_H
