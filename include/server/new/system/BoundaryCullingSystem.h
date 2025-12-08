//
// Created by 15201 on 12/1/2025.
//

#ifndef UNDEROCEAN_BOUNDARYCULLINGSYSTEM_H
#define UNDEROCEAN_BOUNDARYCULLINGSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include "server/new/resources/GridResource.h"

class BoundaryCullinSystem : public ISystem {
private:
    Coordinator& coord_;
public:
    explicit BoundaryCullinSystem(Coordinator& coordinator)
        :coord_(coordinator){
    }
    void update(float dt) override {
        auto& grid = coord_.ctx<GridResource>();
        for (Entity e : grid.outOfBoundEntities){
            coord_.destroyEntity(e);
        }
    }
};
#endif //UNDEROCEAN_BOUNDARYCULLINGSYSTEM_H