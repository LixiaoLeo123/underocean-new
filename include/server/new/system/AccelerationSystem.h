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
        signature_.set(Coordinator::getComponentTypeID<Force>(), true);
        signature_.set(Coordinator::getComponentTypeID<Mass>(), true);  //mass should also exist to calculate acceleration
        //coord_.registerSystem(signature_);
    }

    void update(float dt) override {
        auto& grid = coord_.ctx<GridResource>();
        for (auto& cell : grid.cells_) {
            if (!cell.isAOI) continue;
            for (Entity entity : cell.entities) {
                if (!coord_.hasSignature(entity, signature_)) continue;
                if (coord_.hasComponent<NetworkPeer>(entity)) continue;  //skip player controlled entities
                assert(coord_.getComponent<Mass>(entity).mass != 0.f && "Mass component's mass should not be zero");
                coord_.getComponent<Velocity>(entity) +=
                    static_cast<UVector>(coord_.getComponent<Force>(entity)) / coord_.getComponent<Mass>(entity).mass * dt;
            }
        }
    }
};
#endif //UNDEROCEAN_ACCELERATIONSYSTEM_H