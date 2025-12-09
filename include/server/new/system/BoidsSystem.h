//
// Created by 15201 on 12/1/2025.
//

#ifndef UNDEROCEAN_BOIDSSYSTEM_H
#define UNDEROCEAN_BOIDSSYSTEM_H
#include <SFML/System/Vector2.hpp>

#include "ISystem.h"
#include "common/utils/Physics.h"
#include "server/new/Coordinator.h"
#include "server/new/resources/GridResource.h"
#include "server/new/component/Components.h"
//entities that boids will handle
#define BOIDS_ENTITY_TYPES \
X(SMALL_YELLOW)

class BoidsSystem : public ISystem {
private:
    Signature signature_ {};
    Signature requiredSignature_ {};  //signature that other handled entity should have
    Coordinator& coord_;
    template<EntityTypeID ID>    //though check by signature, we still need x-macro to specify this logic
    void updateFlocking(Entity e, int r, int c) {
        UVector cohesion(0, 0);
        UVector separation(0, 0);
        UVector alignment(0, 0);
        int neighborCount = 0;
        for (int dr = -ParamTable<ID>::PERCEPTION_DIST; dr <= ParamTable<ID>::PERCEPTION_DIST; ++dr) {
            for (int dc = -ParamTable<ID>::PERCEPTION_DIST; dc <= ParamTable<ID>::PERCEPTION_DIST; ++dc) {
                int worldRow = r + dr;
                int worldCol = c + dc;
                if (!coord_.ctx<GridResource>().cellExistAt(worldRow, worldCol)) continue;  //out of bound
                auto& curCell = coord_.ctx<GridResource>().cellAt(worldRow, worldCol);
                for (Entity entity: curCell.entities) {
                    if (!coord_.hasSignature(entity, requiredSignature_)) continue;
                    if (coord_.getComponent<EntityType>(entity).entityID != ID) continue;  //not same type
                    if (entity == e) continue;   //same fish
                    float dist2 = Physics::distance2(coord_.getComponent<Transform>(e), coord_.getComponent<Transform>(entity));
                    if (dist2 < ParamTable<ID>::NEIGHBOR_RADIUS2 && dist2 > 0.0f) {   //group and avoid /0
                        cohesion += coord_.getComponent<Transform>(entity);
                        alignment += coord_.getComponent<Velocity>(entity);
                        ++neighborCount;
                        if (dist2 < ParamTable<ID>::SEPARATION_RADIUS2)   //separate
                            separation += (static_cast<UVector>(coord_.getComponent<Transform>(e)) -
                                coord_.getComponent<Transform>(entity)) / dist2;
                    }
                }
            }
        }
        if (neighborCount) {
            cohesion /= static_cast<float>(neighborCount);
            alignment /= static_cast<float>(neighborCount);
            cohesion -= coord_.getComponent<Transform>(e);
        }
        UVector boidsAcc = cohesion * ParamTable<ID>::COHESION_WEIGHT +
        separation * ParamTable<ID>::SEPARATION_WEIGHT +
        alignment * ParamTable<ID>::ALIGNMENT_WEIGHT;
        coord_.getComponent<Acceleration>(e) = boidsAcc;
    }
public:
    explicit BoidsSystem(Coordinator &coordinator)
        : coord_(coordinator) {
        {
            signature_.set(Coordinator::getComponentTypeID<EntityType>(), true);
            signature_.set(Coordinator::getComponentTypeID<Boids>(), true);
            signature_.set(Coordinator::getComponentTypeID<Transform>(), true);
            signature_.set(Coordinator::getComponentTypeID<Acceleration>(), true);
        }
        {
            requiredSignature_.set(Coordinator::getComponentTypeID<EntityType>(), true);
            requiredSignature_.set(Coordinator::getComponentTypeID<Transform>(), true);
        }
        //a boids entity must have args like ... (see types.h)
        //coord_.registerSystem(signature_);
    }
    void update(float dt) override {
        const auto &updateEntity = [this](Entity e, EntityTypeID id, int r, int c) {
            switch (id) {
#define X(name) case EntityTypeID::name: updateFlocking<EntityTypeID::name>(e, r, c); break;
                BOIDS_ENTITY_TYPES
#undef X
                default: break;
            }
        };
        auto& grid = coord_.ctx<GridResource>();
        for (int r = -1; r <= grid.rows_; ++r) {
            for (int c = -1; c <= grid.cols_; ++c) {
                auto& cell = grid.cellAt(r, c);
                if (!cell.isAOI) continue; //far
                for (Entity e: cell.entities) {
                    if (!coord_.hasSignature(e, signature_)) continue; //dont have boids
                    updateEntity(e, coord_.getComponent<EntityType>(e).entityID, r, c);  //distribute
                }
            }
        }
    }
};
#undef BOIDS_ENTITY_TYPES
#endif //UNDEROCEAN_BOIDSSYSTEM_H