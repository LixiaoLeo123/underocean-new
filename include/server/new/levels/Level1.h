//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVEL1_H
#define UNDEROCEAN_LEVEL1_H
#include "server/new/Coordinator.h"
#include "server/new/LevelBase.h"
#include "server/new/resources/plots/PlotContext1.h"
#include "server/new/system/BoidsSystem.h"
#include "server/new/system/EntityGenerationSystem.h"
#include "server/new/system/NetworkControlSystem.h"

class Level1 final : public LevelBase {
public:
    explicit Level1(GameServer& server);
    void update(float dt) override;
    void onPlayerLeave(ENetPeer *player) override;
    void onPlayerJoin(ENetPeer *player) override;
    void initialize() override {
        emplaceSystem<NetworkControlSystem>(coordinator_, server_, MAP_SIZE);
        LevelBase::initialize();
        emplaceSystem<EntityGenerationSystem>(coordinator_, entityFactory_, MAX_ENTITIES);
    }
    UVector getMapSize() override {
        return MAP_SIZE;
    };
    std::uint16_t ltonX(float x) override {
        float norm = 1 + x / (MAP_SIZE.x) / (1 + 2.f / CHUNK_COLS);
        norm = std::clamp(norm, 0.f, 1.f);
        return static_cast<std::uint16_t>(std::round(norm * 65535.f));
    }
    float ntolX(std::uint16_t x) override {
        float norm = static_cast<float>(x) / 65535.f;
        norm = (norm - 1.f) * (1 + 2.f / CHUNK_COLS);
        return norm * MAP_SIZE.x;
    };
    std::uint16_t ltonY(float y) override {
        float norm = 1 + y / (MAP_SIZE.y) / (1 + 2.f / CHUNK_ROWS);
        norm = std::clamp(norm, 0.f, 1.f);
        return static_cast<std::uint16_t>(std::round(norm * 65535.f));
    };
    float ntolY(std::uint16_t y) override {
        float norm = static_cast<float>(y) / 65535.f;
        norm = (norm - 1.f) * (1 + 2.f / CHUNK_ROWS);
        return norm * MAP_SIZE.y;
    }
protected:
    void customInitialize() override {
        emplaceSystem<BoidsSystem>(coordinator_);
    };
private:
    static constexpr int MAX_ENTITIES = 500;
    static constexpr UVector MAP_SIZE{1280.f, 720.f};  //decided by bg
    static constexpr int CHUNK_ROWS = 15;   //about 50 x 50 px
    static constexpr int CHUNK_COLS = 26;
    Signature networkSignature_{};
};
inline Level1::Level1(GameServer& server): LevelBase(server) {
    LevelBase::initialize();
    coordinator_.ctx<GridResource>().init(MAP_SIZE.x, MAP_SIZE.y, CHUNK_COLS, CHUNK_ROWS);  //40x36 chunks
    coordinator_.emplaceContext<PlotContext1>();
    entityFactory_.addWeightedEntry(EntityTypeID::SMALL_YELLOW, 1);
    networkSignature_.set(Coordinator::getComponentTypeID<NetworkPeer>());
    coordinator_.registerSystem(networkSignature_);
}
inline void Level1::onPlayerLeave(ENetPeer* player) {
    auto& peerEntities = coordinator_.getEntitiesWith(networkSignature_);
    for (Entity e : peerEntities) {
        if (coordinator_.getComponent<NetworkPeer>(e).peer == player) {
            coordinator_.destroyEntity(e);  //do not break to prevent multiple player bug ?
            break;
        }
    }
}
inline void Level1::onPlayerJoin(ENetPeer* player) {   //call by GameServer
    auto& playerData = server_.playerList_[player];
    Entity playerEntity = entityFactory_.spawnWithID(playerData.type, true);
    coordinator_.addComponent(playerEntity, NetworkPeer{player});
    coordinator_.notifyEntityChanged(playerEntity);
}
inline void Level1::update(float dt) {
    LevelBase::update(dt);
}
#endif //UNDEROCEAN_LEVEL1_H
