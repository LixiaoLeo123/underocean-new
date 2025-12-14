//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_MOVEMENTSYSTEM_H
#define UNDEROCEAN_MOVEMENTSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include "server/new/resources/GridResource.h"

class MovementSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord_;
public:
    explicit MovementSystem(Coordinator& coordinator)
        :coord_(coordinator){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Velocity>()), true);
        //coord_.registerSystem(signature_);
    }
    void update(float dt) override {
        auto& grid = coord_.ctx<GridResource>();
        for (auto& cell : grid.cells_) {
            if (!cell.isAOI) continue;
            for (Entity entity : cell.entities) {
                if (!coord_.hasSignature(entity, signature_)) continue;
                if (coord_.hasComponent<NetworkPeer>(entity)) continue;  //skip player controlled entities
                coord_.getComponent<Transform>(entity) +=
                    static_cast<UVector>(coord_.getComponent<Velocity>(entity)) * dt;
            }
        }
    }
};
#endif //UNDEROCEAN_MOVEMENTSYSTEM_H