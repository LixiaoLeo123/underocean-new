//
// Created by 15201 on 12/17/2025.
//

#ifndef UNDEROCEAN_COLLISIONSYSTEM_H
#define UNDEROCEAN_COLLISIONSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include <span>

class EventBus;

class CollisionSystem : public ISystem {
private:
    Signature collisionSig_;
    Coordinator& coord_;
    EventBus& eventBus_;
public:
    explicit CollisionSystem(Coordinator& coord, EventBus& eventBus) : coord_(coord), eventBus_(eventBus) {
        {
            collisionSig_.set(Coordinator::getComponentTypeID<Collision>());
            collisionSig_.set(Coordinator::getComponentTypeID<Size>());
            collisionSig_.set(Coordinator::getComponentTypeID<Transform>());
            collisionSig_.set(Coordinator::getComponentTypeID<EntityType>());  //to get default hitbox(es)
            collisionSig_.set(Coordinator::getComponentTypeID<Velocity>());  //to get hitbox rotation
        }
    }
    void update(float dt) override;
    static constexpr std::span<const HitBox> getOriginalHitBoxes(EntityTypeID type) {
        switch (type) {
#define X(t) \
            case EntityTypeID::t: \
                return std::span{ParamTable<EntityTypeID::t>::HIT_BOXES};
            PLAYER_ENTITY_TYPES
#undef X
        default:
            return {};
        }
    }
};
#endif //UNDEROCEAN_COLLISIONSYSTEM_H