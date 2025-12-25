//
// Created by 15201 on 12/17/2025.
//

#ifndef UNDEROCEAN_ENTITYCLEARSYSTEM_H
#define UNDEROCEAN_ENTITYCLEARSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include "server/new/EventBus.h"
class Coordinator;

class EntityClearSystem : public ISystem {
private:
    Coordinator& coord_;
    Signature clearSig_;
    EventBus& eventBus_;
public:
    explicit EntityClearSystem(Coordinator& coordinator, EventBus& eventBus) : coord_(coordinator), eventBus_(eventBus) {
        {
            clearSig_.set(Coordinator::getComponentTypeID<EntityClearTag>());
            coord_.registerSystem(clearSig_);
        }
        eventBus.subscribe<EntityDeathEvent>([this](const EntityDeathEvent& event) {
            this->onEntityDeath(event);
        });
    }
    void update(float dt) override;
    void onEntityDeath(const EntityDeathEvent& event) const;
};
inline void EntityClearSystem::onEntityDeath(const EntityDeathEvent &event) const {
    Entity e = event.entity;
    if (!coord_.hasComponent<NetworkPeer>(event.entity)) {  //not a player entity
        coord_.addComponent<EntityClearTag>(e, {});
        coord_.notifyEntityChanged(e);
    }
    else {
        assert(coord_.hasComponent<Transform>(event.entity));
        auto& transform = coord_.getComponent<Transform>(event.entity);
        transform.x = transform.y = -65535.f;
        auto& velocity = coord_.getComponent<Velocity>(event.entity);
        velocity.vx = velocity.vy = 0.f;
        coord_.getComponent<Invincibility>(event.entity).cooldown = 6.f;
        coord_.addComponent<NoNetControl>(event.entity, {5.5f});
        coord_.notifyEntityChanged(event.entity);
        eventBus_.publish<PlayerRespawnEvent>({e});
    }
}
inline void EntityClearSystem::update(float dt) {
    auto entities = coord_.getEntitiesWith(clearSig_);
    for (Entity e : entities) {
        coord_.destroyEntity(e);
    }
}

#endif //UNDEROCEAN_ENTITYCLEARSYSTEM_H