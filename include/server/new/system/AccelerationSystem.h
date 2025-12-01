//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_ACCELERATIONSYSTEM_H
#define UNDEROCEAN_ACCELERATIONSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"

class AccelerationSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord_;
public:
    explicit AccelerationSystem(Coordinator& coordinator)
        :coord_(coordinator){
        signature_.set(Coordinator::getComponentTypeID<Velocity>(), true);
        signature_.set(Coordinator::getComponentTypeID<Acceleration>(), true);
        coord_.registerSystem(signature_);
    }

    void update(float dt) override {
        const auto& entities = coord_.getEntitiesWith(signature_);
        for (Entity e : entities) {
            auto& vel = coord_.getComponent<Velocity>(e);
            auto& acc = coord_.getComponent<Acceleration>(e);
            vel.vx += acc.ax * dt;
            vel.vy += acc.ay * dt;
        }
    }
};
#endif //UNDEROCEAN_ACCELERATIONSYSTEM_H