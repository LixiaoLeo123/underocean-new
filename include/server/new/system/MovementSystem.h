//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_MOVEMENTSYSTEM_H
#define UNDEROCEAN_MOVEMENTSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"

class MovementSystem : public ISystem {
private:
    Signature signature;
    Coordinator& coord;
public:
    explicit MovementSystem(Coordinator& coordinator)
        :coord(coordinator){
        signature.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        signature.set(static_cast<size_t>(Coordinator::getComponentTypeID<Velocity>()), true);
        coord.registerSystem(signature);
    }
    void update(float dt) override {
        const auto& entities = coord.getEntitiesWith(signature);
        for (Entity e : entities) {
            auto& pos = coord.getComponent<Transform>(e);
            auto& vel = coord.getComponent<Velocity>(e);
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    }
};
#endif //UNDEROCEAN_MOVEMENTSYSTEM_H