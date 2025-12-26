//
// Created by 15201 on 12/26/2025.
//

#ifndef UNDEROCEAN_TIMESYSTEM_H
#define UNDEROCEAN_TIMESYSTEM_H
#include "ISystem.h"
#include "server/GameServer.h"
#include "server/new/Coordinator.h"
#include "server/new/Entity/EntityFactory.h"
#include "server/new/resources/TimeResource.h"
template <EntityTypeID ID>
concept HasChaseRadius = requires {
    ParamTable<ID>::CHASE_RADIUS;
};
class TimeSystem : public ISystem {
private:
    constexpr static float NIGHT_MONSTER_SPAWN_SCALE = 4.f;
    Coordinator& coord_;
    GameServer& server_;
    EntityFactory& entityFactory_;
    EventBus& eventBus_;
    int curLevel_;
    void changeMonstersSpawnWeight(float scale) {
        for (auto& entry : entityFactory_.weightedEntries_) {
            bool isMonster = false;
#define X(type) if constexpr (HasChaseRadius<EntityTypeID::type>) { isMonster = true; }
            ENTITY_TYPES
#undef X
            if (isMonster) {
                entry.weight *= scale;
            }
        }
    }
    void onSinglePlayerRespawn(const PlayerRespawnEvent& event) {
        coord_.ctx<TimeResource>().reset();
    }
public:
    explicit TimeSystem(Coordinator& coord, GameServer& server, EntityFactory& entityFactory, EventBus& eventBus, ILevel& level) :
        coord_(coord) , server_(server), entityFactory_(entityFactory), eventBus_(eventBus) {
        eventBus_.subscribe<PlayerRespawnEvent>([this](const PlayerRespawnEvent& event) {
            this->onSinglePlayerRespawn(event);
        });
        curLevel_ = level.getLevel();
    }
    void update(float dt) override {
        auto&[currentTime] = coord_.ctx<TimeResource>();
        if (currentTime < MAX_TIME / 2.f && currentTime + dt >= MAX_TIME / 2.f){
            server_.broadcastInLevel("&c&oDarker, darker, yet darker...", curLevel_);
            changeMonstersSpawnWeight(NIGHT_MONSTER_SPAWN_SCALE);
        }
        currentTime += dt;
        if (currentTime >= MAX_TIME) {
            currentTime = 0.f;
            server_.broadcastInLevel("&a&oIt's dawn...", curLevel_);
            changeMonstersSpawnWeight(1.f / NIGHT_MONSTER_SPAWN_SCALE);
        }
    }
};
#endif //UNDEROCEAN_TIMESYSTEM_H