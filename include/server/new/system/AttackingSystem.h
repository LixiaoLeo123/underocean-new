//
// Created by 15201 on 12/17/2025.
//

#ifndef UNDEROCEAN_ATTACKINGSYSTEM_H
#define UNDEROCEAN_ATTACKINGSYSTEM_H
#include "ISystem.h"
#include "server/new/Coordinator.h"
#include "server/new/EventBus.h"
#include "server/new/component/Components.h"

struct EntityCollisionEvent;

class AttackingSystem : public ISystem {
private:
    constexpr static float INVINCIBILITY_DURATION = 0.25f;  //1 second invincibility after being attacked
    Coordinator& coord_;
    Signature invincibilitySig_{};  //to update cooldowns
    EventBus& eventBus_;
public:
    explicit AttackingSystem(EventBus& eventBus, Coordinator& coord)
        :eventBus_(eventBus), coord_(coord){
        eventBus.subscribe<EntityCollisionEvent>([this](const EntityCollisionEvent& event) {
            this->onEntityCollision(event);
        });
        {
            invincibilitySig_.set(Coordinator::getComponentTypeID<Invincibility>());
        }
    }
    void update(float dt) override {
        auto& entities = coord_.getEntitiesWith(invincibilitySig_);
        for (Entity e : entities) {
            auto& invincibilityComp = coord_.getComponent<Invincibility>(e);
            if (invincibilityComp.cooldown > 0.f) {
                invincibilityComp.cooldown -= dt;
                if (invincibilityComp.cooldown < 0.f) {
                    invincibilityComp.cooldown = 0.f;
                }
            }
        }
    }
    void onEntityCollision(const EntityCollisionEvent& event);
};
#endif //UNDEROCEAN_ATTACKINGSYSTEM_H