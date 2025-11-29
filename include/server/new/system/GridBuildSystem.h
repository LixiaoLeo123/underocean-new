//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_GRIDBUILDSYSTEM_H
#define UNDEROCEAN_GRIDBUILDSYSTEM_H
#include "ISystem.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include "server/new/resources/GridResource.h"

class GridBuildSystem : public ISystem {  //using GridResource
public:
    Signature signature_ {};  //only with transform
    Coordinator& coord_;
    explicit GridBuildSystem(Coordinator& coordinator)  //build grid for all signatures
        :coord_(coordinator){
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        coord_.registerSystem(signature_);
    }
    void update(float dt) override;;
};
inline void GridBuildSystem::update(float dt) {
    GridResource& grid = coord_.ctx<GridResource>();
    const auto& entities = coord_.getEntitiesWith(signature_);
    for (Entity e : entities) {
        Transform pos = coord_.getComponent<Transform>(e);
        grid.insert(e, pos.x, pos.y);
    }
}
#endif //UNDEROCEAN_GRIDBUILDSYSTEM_H