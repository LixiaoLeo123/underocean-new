//
// Created by 15201 on 12/1/2025.
//

#ifndef UNDEROCEAN_VELOCITYLIMITSYSTEM_H
#define UNDEROCEAN_VELOCITYLIMITSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"


class VelocityLimitSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord;
public:
    explicit VelocityLimitSystem(Coordinator& coordinator)
        :coord(coordinator){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<MaxVelocity>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Velocity>()), true);
        //coord.registerSystem(signature_);
    }
    void update(float dt) override {
        auto &grid = coord.ctx<GridResource>();
        for (auto &cell: grid.cells_) {
            if (!cell.isAOI) continue;
            for (Entity entity: cell.entities) {
                float maxVelocity = coord.getComponent<MaxVelocity>(entity).maxVelocity;
                auto &vel = coord.getComponent<Velocity>(entity);
                float speed2 = vel.vx * vel.vx + vel.vy * vel.vy;
                if (speed2 > maxVelocity * maxVelocity && speed2 > 0.0f) {
                    float ratio = maxVelocity / std::sqrt(speed2);
                    vel.vx *= ratio;
                    vel.vy *= ratio;
                }
            }
        }
    }
};
#endif //UNDEROCEAN_VELOCITYLIMITSYSTEM_H