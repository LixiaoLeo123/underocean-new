#include "server/new/levels/Level5.h"

#include "server/new/resources/TimeResource.h"
#include "server/new/resources/plots/PlotContext1.h"
#include "server/new/system/AccelerationLimitSystem.h"
#include "server/new/system/AccelerationSystem.h"
#include "server/new/system/AttackingSystem.h"
#include "server/new/system/BoidsSystem.h"
#include "server/new/system/BoundaryCullingSystem.h"
#include "server/new/system/ChasingSystem.h"
#include "server/new/system/CollisionSystem.h"
#include "server/new/system/DerivedAttributeSystem.h"
#include "server/new/system/EntityClearSystem.h"
#include "server/new/system/GridBuildSystem.h"
#include "server/new/system/SkillSystem.h"
#include "server/new/system/VelocityLimitSystem.h"
#include "server/new/system/TimeSystem.h"


void Level5::initialize() {
    coordinator_.emplaceContext<GridResource>();
    coordinator_.emplaceContext<TimeResource>();
    emplaceSystem<GridBuildSystem>(coordinator_);
    emplaceSystem<TimeSystem>(coordinator_, server_, entityFactory_, eventBus_, *this);
    emplaceSystem<DerivedAttributeSystem>(coordinator_, eventBus_);
    emplaceSystem<NetworkControlSystem>(coordinator_, server_, *this, eventBus_);
    emplaceSystem<BoidsSystem>(coordinator_);
    emplaceSystem<ChasingSystem>(coordinator_, eventBus_);
    emplaceSystem<SkillSystem>(server_, coordinator_, eventBus_);
    emplaceSystem<AttackingSystem>(eventBus_, coordinator_);
    emplaceSystem<CollisionSystem>(coordinator_, eventBus_);
    emplaceSystem<EntityGenerationSystem>(coordinator_, entityFactory_, MAX_ENTITIES);
    emplaceSystem<BoundaryCullingSystem>(coordinator_);
    emplaceSystem<AccelerationLimitSystem>(coordinator_);
    emplaceSystem<AccelerationSystem>(coordinator_);
    emplaceSystem<VelocityLimitSystem>(coordinator_);
    emplaceSystem<MovementSystem>(coordinator_);
    emplaceSystem<NetworkSyncSystem>(coordinator_, server_, *this, eventBus_);
    emplaceSystem<EntityClearSystem>(coordinator_, eventBus_);
    getSystem<EntityGenerationSystem>().setGenerationSpeed(10000.f);  //fast spawn
    coordinator_.ctx<GridResource>().init(MAP_SIZE.x, MAP_SIZE.y, CHUNK_COLS, CHUNK_ROWS);  //40x36 chunks
    coordinator_.emplaceContext<PlotContext1>();
    entityFactory_.setSpawnArea({0.f, 0.f}, {MAP_SIZE.x, MAP_SIZE.y});
    entityFactory_.addWeightedEntry(EntityTypeID::FLY_FISH, 0.3);
    entityFactory_.addWeightedEntry(EntityTypeID::RED_LIGHT, 0.3);
    entityFactory_.addWeightedEntry(EntityTypeID::TURTLE, 0.1);
    entityFactory_.addWeightedEntry(EntityTypeID::UGLY_FISH, 0.4);
    entityFactory_.addWeightedEntry(EntityTypeID::SMALL_YELLOW, 0.3);
    entityFactory_.addWeightedEntry(EntityTypeID::BALL_ORANGE, 0.2);
    entityFactory_.addWeightedEntry(EntityTypeID::BLUE_LONG, 0.1);
    entityFactory_.addWeightedEntry(EntityTypeID::ROUND_GREEN, 2);
    entityFactory_.addWeightedEntry(EntityTypeID::SMALL_SHARK, 0.1);
    networkSignature_.set(Coordinator::getComponentTypeID<NetworkPeer>());
    coordinator_.registerSystem(networkSignature_);
}
