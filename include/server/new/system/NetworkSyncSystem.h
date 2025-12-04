//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_NETWORKSYNCSYSTEM_H
#define UNDEROCEAN_NETWORKSYNCSYSTEM_H
#include "ISystem.h"

class NetworkSyncSystem : public ISystem {
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
                coord_.getComponent<Transform>(entity) +=
                    static_cast<UVector>(coord_.getComponent<Velocity>(entity)) * dt;
            }
        }
    }
};
#endif //UNDEROCEAN_NETWORKSYNCSYSTEM_H