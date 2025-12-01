//
// Created by 15201 on 12/1/2025.
//

#ifndef UNDEROCEAN_ACCELERATIONLIMITSYSTEM_H
#define UNDEROCEAN_ACCELERATIONLIMITSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
class AccelerationLimitSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord;
public:
    explicit AccelerationLimitSystem(Coordinator& coordinator)
        :coord(coordinator){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<MaxAcceleration>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Acceleration>()), true);
        coord.registerSystem(signature_);
    }
    void update(float dt) override {
        const auto& entities = coord.getEntitiesWith(signature_);
        for (Entity e : entities) {
            float maxAcceleration = coord.getComponent<MaxAcceleration>(e).maxAcceleration;
            auto& acc = coord.getComponent<Acceleration>(e);
            float acc2 = acc.ax * acc.ax + acc.ay * acc.ay;
            if (acc2 > maxAcceleration * maxAcceleration && acc2 > 0.0f) {
                float ratio = maxAcceleration / std::sqrt(acc2);
                acc.ax *= ratio;
                acc.ay *= ratio;
            }
        }
    }
};
#endif //UNDEROCEAN_ACCELERATIONLIMITSYSTEM_H