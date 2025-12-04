//
// Created by 15201 on 12/1/2025.
//

#ifndef UNDEROCEAN_BOUNDARYCULLINGSYSTEM_H
#define UNDEROCEAN_BOUNDARYCULLINGSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"

class MovementSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord;
public:
    explicit MovementSystem(Coordinator& coordinator)
        :coord(coordinator){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        coord.registerSystem(signature_);   //only transform
    }
    void update(float dt) override {
        const auto& entities = coord.getEntitiesWith(signature_);
        for (Entity e : entities) {
            auto& pos = coord.getComponent<Transform>(e);
            auto& vel = coord.getComponent<Velocity>(e);
            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
        }
    }
};
#endif //UNDEROCEAN_BOUNDARYCULLINGSYSTEM_H