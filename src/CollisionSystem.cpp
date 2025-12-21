#include "server/new/system/CollisionSystem.h"

#include "common/utils/Physics.h"
#include "server/new/EventBus.h"
#include "server/new/resources/GridResource.h"

void CollisionSystem::update(float dt) {
    auto& grid = coord_.ctx<GridResource>();
    for (int r = -1; r <= grid.rows_; ++r) {
        for (int c = -1; c <= grid.cols_; ++c) {
            if (!grid.cellAt(r, c).isAOI) continue;
            auto& cell = grid.cellAt(r, c);
            for (auto it = cell.entities.begin(); it != cell.entities.end(); ++it) {
                Entity e1 = *it;
                float size1 = coord_.getComponent<Size>(e1).size;
                auto& trans1 = coord_.getComponent<Transform>(e1);
                for (int dc = -1; dc <= 1; ++dc) {  //neighbor cells, we assert that an entity wouldn't cross more than 1 cell per frame
                    for (int dr = -1; dr <= 1; ++dr) {
                        int neighborC = c + dc;
                        int neighborR = r + dr;
                        if (!grid.cellExistAt(neighborR, neighborC)) continue;  //out of bound
                        auto& neighborCell = grid.cellAt(neighborR, neighborC);
                        if (!neighborCell.isAOI) continue; //not aoi
                        for (auto it2 = neighborCell.entities.begin(); it2 != neighborCell.entities.end(); ++it2) {
                            Entity e2 = *it2;
                            if (e2 <= e1) continue;  //avoid duplicate check
                            float size2 = coord_.getComponent<Size>(e2).size;
                            auto& trans2 = coord_.getComponent<Transform>(e2);
                            float minDis = (size1 + size2) / 2;
                            if (std::abs(trans1.x - trans2.x) >= minDis ||
                                std::abs(trans1.y - trans2.y) >= minDis) {
                                continue;  //by rect quick check
                            }
                            if (Physics::distance2(trans1, trans2) >= minDis * minDis) {
                                continue;  //by circle quick check
                            }
                            const auto& coll1 = getOriginalHitBoxes(coord_.getComponent<EntityType>(e1).entityID);
                            const auto& coll2 = getOriginalHitBoxes(coord_.getComponent<EntityType>(e2).entityID);
                            float rot1 = std::atan2f(
                                coord_.getComponent<Velocity>(e1).vy,
                                coord_.getComponent<Velocity>(e1).vx
                            );
                            float rot2 = std::atan2f(
                                coord_.getComponent<Velocity>(e2).vy,
                                coord_.getComponent<Velocity>(e2).vx
                            );
                            if (Physics::checkCollisionByOBB(coll1, coll2,
                                trans1, rot1, trans2, rot2, size1, size2)) {
                                //collision detected between e1 and e2
                                eventBus_.publish<EntityCollisionEvent>({e1, e2});
                            }
                        }
                    }
                }
            }
        }
    }
}
