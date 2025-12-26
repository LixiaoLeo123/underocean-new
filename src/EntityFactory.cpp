#include "server/new/Entity/EntityFactory.h"

#include "common/utils/Random.h"
#include "server/new/EventBus.h"
#include "server/new/component/Components.h"

void EntityFactory::initialize(EventBus& eventBus) {
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
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::SMALL_YELLOW>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::SMALL_YELLOW>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::SMALL_YELLOW};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::SMALL_YELLOW>::INIT_SIZE
                    + ParamTable<EntityTypeID::SMALL_YELLOW>::SIZE_STEP * static_cast<float>(Random::randInt(0, 1))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
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
        registerSpawner(EntityTypeID::FLY_FISH, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::FLY_FISH>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::FLY_FISH>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::FLY_FISH};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::FLY_FISH>::INIT_SIZE
                    + ParamTable<EntityTypeID::FLY_FISH>::SIZE_STEP * static_cast<float>(Random::randInt(0, 10))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
        registerSpawner(EntityTypeID::RED_LIGHT, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::RED_LIGHT>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::RED_LIGHT>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::RED_LIGHT};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::RED_LIGHT>::INIT_SIZE
                    + ParamTable<EntityTypeID::RED_LIGHT>::SIZE_STEP * static_cast<float>(Random::randInt(0, 10))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
        registerSpawner(EntityTypeID::SMALL_SHARK, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::SMALL_SHARK>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::SMALL_SHARK>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::SMALL_SHARK};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Chase{false, ParamTable<EntityTypeID::SMALL_SHARK>::CHASE_RADIUS, 0});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::SMALL_SHARK>::INIT_SIZE
                    + ParamTable<EntityTypeID::SMALL_SHARK>::SIZE_STEP * static_cast<float>(Random::randInt(3, 80))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
        });
    registerSpawner(EntityTypeID::TURTLE, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::TURTLE>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::TURTLE>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::TURTLE};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::TURTLE>::INIT_SIZE
                    + ParamTable<EntityTypeID::TURTLE>::SIZE_STEP * static_cast<float>(Random::randInt(3, 80))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
        registerSpawner(EntityTypeID::BALL_ORANGE, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::BALL_ORANGE>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::BALL_ORANGE>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::BALL_ORANGE};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::BALL_ORANGE>::INIT_SIZE
                    + ParamTable<EntityTypeID::BALL_ORANGE>::SIZE_STEP * static_cast<float>(Random::randInt(0, 1))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
        registerSpawner(EntityTypeID::BLUE_LONG, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::BLUE_LONG>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::BLUE_LONG>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::BLUE_LONG};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::BLUE_LONG>::INIT_SIZE
                    + ParamTable<EntityTypeID::BLUE_LONG>::SIZE_STEP * static_cast<float>(Random::randInt(0, 1))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
        registerSpawner(EntityTypeID::ROUND_GREEN, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::ROUND_GREEN>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::ROUND_GREEN>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::ROUND_GREEN};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::ROUND_GREEN>::INIT_SIZE
                    + ParamTable<EntityTypeID::ROUND_GREEN>::SIZE_STEP * static_cast<float>(Random::randInt(0, 1))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
        registerSpawner(EntityTypeID::UGLY_FISH, [&](PlayerData* playerData)->Entity {
        Entity newEntity = coord_.createEntity();
        {  //transform
            Transform transform{Random::randFloat(spawnAreaFrom_.x, spawnAreaTo_.x),
                Random::randFloat(spawnAreaFrom_.y, spawnAreaTo_.y)};
            coord_.addComponent(newEntity, transform);
        }
        {  //Force
            coord_.addComponent(newEntity, Force{});
        }
        {  //max velocity (can change)
            MaxVelocity maxVelocity = {ParamTable<EntityTypeID::UGLY_FISH>::MAX_VELOCITY};
            coord_.addComponent(newEntity, maxVelocity);
        }
        {  //random velocity
            sf::Vector2f vecVelocity = Random::randUnitVector() * Random::randFloat(0.f,
                                           ParamTable<EntityTypeID::UGLY_FISH>::MAX_VELOCITY);
            coord_.addComponent(newEntity, Velocity{vecVelocity.x, vecVelocity.y});
        }
        {  //entity type
            EntityType entityType = {EntityTypeID::UGLY_FISH};
            coord_.addComponent(newEntity, entityType);
        }
        {  //collision
            coord_.addComponent<Collision>(newEntity, {});
        }
        coord_.addComponent<NetSyncComp>(newEntity, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
        if (playerData) {
            coord_.addComponent(newEntity, ForceLoadChunk{});
            coord_.addComponent(newEntity, NetworkPeer{playerData->peer});
            {  //size
                coord_.addComponent<Size>(newEntity, {playerData->size});
            }
            {  //defence
                coord_.addComponent<Defence>(newEntity, {});
            }
            {
                coord_.addComponent<Invincibility>(newEntity, {3.f});
            }
            {  //player id
                NameTag nameTag(playerData->playerId);
                coord_.addComponent<NameTag>(newEntity, nameTag);
            }
        }
        else {
            coord_.addComponent(newEntity, Boids{});
            {
                coord_.addComponent<Invincibility>(newEntity, {0.f});
            }
            {  //size
                Size size = {
                    ParamTable<EntityTypeID::UGLY_FISH>::INIT_SIZE
                    + ParamTable<EntityTypeID::UGLY_FISH>::SIZE_STEP * static_cast<float>(Random::randInt(0, 1))
                };
                coord_.addComponent(newEntity, size);
            }
        }
        eventBus.publish<AttributedEntityInitEvent>({newEntity, static_cast<bool>(playerData),
            static_cast<bool>(playerData), static_cast<bool>(playerData)});
        coord_.notifyEntityChanged(newEntity);
        return newEntity;
    });
}
