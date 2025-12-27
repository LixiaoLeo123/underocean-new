//
// Created by 15201 on 12/27/2025.
//

#ifndef UNDEROCEAN_LEVEL2_H
#define UNDEROCEAN_LEVEL2_H
#include "server/new/Coordinator.h"
#include "server/new/LevelBase.h"
#include "server/new/resources/TimeResource.h"
#include "server/new/system/AccelerationSystem.h"
#include "server/new/system/EntityGenerationSystem.h"
#include "server/new/system/NetworkControlSystem.h"
#include "server/new/system/NetworkSyncSystem.h"

class Level2 final : public LevelBase {
public:
    explicit Level2(GameServer& server): LevelBase(server) {
        initialize();
    }
    void update(float dt) override;
    void onPlayerLeave(PlayerData& playerData) override;
    void onPlayerJoin(PlayerData& playerData) override;
    void initialize() override;
    UVector getMapSize() override {
        return MAP_SIZE;
    };
    std::uint16_t ltonX(float x) override {
        float norm = 1.f / (2 + CHUNK_COLS) + x / (MAP_SIZE.x) / (1 + 2.f / CHUNK_COLS);
        norm = std::clamp(norm, 0.f, 1.f);
        return static_cast<std::uint16_t>(std::round(norm * 65535.f));
    }
    float ntolX(std::uint16_t x) override {
        float norm = static_cast<float>(x) / 65535.f;
        float offset = 1.f / (2 + CHUNK_COLS);
        norm = (norm - offset) * (1 + 2.f / CHUNK_COLS);
        return norm * MAP_SIZE.x;
    }
    float ntolY(std::uint16_t y) override {
        float norm = static_cast<float>(y) / 65535.f;
        float offset = 1.f / (2 + CHUNK_ROWS);
        norm = (norm - offset) * (1 + 2.f / CHUNK_ROWS);
        return norm * MAP_SIZE.y;
    }
    std::uint16_t ltonY(float y) override {
        float norm = 1.f / (2 + CHUNK_ROWS) + y / (MAP_SIZE.y) / (1 + 2.f / CHUNK_ROWS);
        norm = std::clamp(norm, 0.f, 1.f);
        return static_cast<std::uint16_t>(std::round(norm * 65535.f));
    }
    [[nodiscard]] int getLevelID() const override { return 1; };
    void handleRequestRespawn(ENetPeer *peer) override {
        auto& peerEntities = coordinator_.getEntitiesWith(networkSignature_);
        for (Entity e : peerEntities) {
            if (coordinator_.getComponent<NetworkPeer>(e).peer == peer) {
                eventBus_.publish<PlayerRespawnEvent>({e});
            }
        }
    }
    int getLevel() override { return 2; }
protected:
    // void customInitialize() override {
    //     emplaceSystem<BoidsSystem>(coordinator_);
    // };
private:
    static constexpr int MAX_ENTITIES = 30000;
    static constexpr UVector MAP_SIZE{3840.f, 1080.f};  //decided by bg
    static constexpr int CHUNK_ROWS = 68;   //about 16 x 16 px  15, 26
    static constexpr int CHUNK_COLS = 240;
    Signature networkSignature_{};
};
inline void Level2::onPlayerLeave(PlayerData& playerData) {
    auto& peerEntities = coordinator_.getEntitiesWith(networkSignature_);
    for (Entity e : peerEntities) {
        if (coordinator_.getComponent<NetworkPeer>(e).peer == playerData.peer) {
            eventBus_.publish<PlayerLeaveEvent>({coordinator_.getComponent<NetworkPeer>(e).peer});
            playerData.initHP = coordinator_.getComponent<HP>(e).hp;
            playerData.initFP = coordinator_.getComponent<FP>(e).fp;
            playerData.size = coordinator_.getComponent<Size>(e).size;
            coordinator_.destroyEntity(e);  //do not break to prevent multiple player bug ?
            break;
        }
    }
}
inline void Level2::onPlayerJoin(PlayerData& playerData) {   //call by GameServer
    entityFactory_.spawnPlayerEntity(&playerData);
    eventBus_.publish<PlayerJoinEvent>({playerData});
    if (!server_.isMultiplePlayer()) {
        coordinator_.ctx<TimeResource>().reset();
    }
}
inline void Level2::update(float dt) {
    LevelBase::update(dt);
}
#endif //UNDEROCEAN_LEVEL2_H