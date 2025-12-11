#include "server/new/Entity/EntityFactory.h"

#include "common/utils/Random.h"
#include "server/new/component/Components.h"

void EntityFactory::initialize() {
    registerSpawner(EntityTypeID::SMALL_YELLOW, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //mass
            coord_.addComponent<Mass>(newEntity, {ParamTable<EntityTypeID::SMALL_YELLOW>::MASS});
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::SMALL_YELLOW>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::SMALL_YELLOW>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //max acceleration
            MaxAcceleration maxAcceleration = {ParamTable<EntityTypeID::SMALL_YELLOW>::MAX_ACCELERATION};
            coord_.addComponent(newEntity, maxAcceleration);
        }
        {  //size
            Size size = {ParamTable<EntityTypeID::SMALL_YELLOW>::INIT_SIZE};
            coord_.addComponent(newEntity, size);
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::SMALL_YELLOW};
            coord_.addComponent(newEntity, entityType);
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.getComponent<Velocity>(newEntity) = static_cast<UVector>(coord_.getComponent<Velocity>(newEntity)) * 2;
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
        }
        else {
            coord_.addComponent(newEntity, Boids{});
        }
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
    // registerSpawner(EntityTypeID::UGLY_FISH, [&](bool isPlayer)->Entity {
    //     Entity newEntity = coord_.createEntity();
    //     {  //transform
    //         Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
    //             Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
    //         coord_.addComponent(newEntity, transform);
    //     }
    //     {  //entity type
    //         EntityType entityType = {EntityTypeID::UGLY_FISH};
    //         coord_.addComponent(newEntity, entityType);
    //     }
    //     {  //max velocity (can change)
    //         coord_.addComponent(newEntity, ParamTable<EntityTypeID::UGLY_FISH>::MAX_VELOCITY);
    //     }
    //     {  //random velocity
    //         sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
    //             ParamTable<EntityTypeID::UGLY_FISH>::MAX_VELOCITY);
    //         coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
    //     }
    //     coord_.notifyEntityChanged(newEntity);
    //     return newEntity;
    // });
}
