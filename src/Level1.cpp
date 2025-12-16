//
// Created by 15201 on 11/27/2025.
//
#include "server/new/levels/Level1.h"

#include "server/new/resources/plots/PlotContext1.h"
#include "server/new/system/AccelerationLimitSystem.h"
#include "server/new/system/AccelerationSystem.h"
#include "server/new/system/BoidsSystem.h"
#include "server/new/system/BoundaryCullingSystem.h"
#include "server/new/system/DerivedAttributeSystem.h"
#include "server/new/system/GridBuildSystem.h"
#include "server/new/system/SkillSystem.h"
#include "server/new/system/VelocityLimitSystem.h"

void Level1::initialize() {
    coordinator_.emplaceContext<GridResource>();
    emplaceSystem<DerivedAttributeSystem>(coordinator_, eventBus_);
    emplaceSystem<NetworkControlSystem>(coordinator_, server_, *this, eventBus_);
    emplaceSystem<GridBuildSystem>(coordinator_);
    emplaceSystem<BoidsSystem>(coordinator_);
    emplaceSystem<SkillSystem>(server_, coordinator_, eventBus_);
    emplaceSystem<EntityGenerationSystem>(coordinator_, entityFactory_, MAX_ENTITIES);
    emplaceSystem<BoundaryCullingSystem>(coordinator_);
    emplaceSystem<AccelerationLimitSystem>(coordinator_);
    emplaceSystem<AccelerationSystem>(coordinator_);
    emplaceSystem<VelocityLimitSystem>(coordinator_);
    emplaceSystem<MovementSystem>(coordinator_);
    emplaceSystem<NetworkSyncSystem>(coordinator_, server_, *this, eventBus_);
    getSystem<EntityGenerationSystem>().setGenerationSpeed(10000.f);  //fast spawn
    coordinator_.ctx<GridResource>().init(MAP_SIZE.x, MAP_SIZE.y, CHUNK_COLS, CHUNK_ROWS);  //40x36 chunks
    coordinator_.emplaceContext<PlotContext1>();
    entityFactory_.setSpawnArea({0.f, 0.f}, {MAP_SIZE.x, MAP_SIZE.y});
    entityFactory_.addWeightedEntry(EntityTypeID::SMALL_YELLOW, 1);
    networkSignature_.set(Coordinator::getComponentTypeID<NetworkPeer>());
    coordinator_.registerSystem(networkSignature_);
}
