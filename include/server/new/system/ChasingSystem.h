#ifndef UNDEROCEAN_CHASINGSYSTEM_H
#define UNDEROCEAN_CHASINGSYSTEM_H

#include "ISystem.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include "server/new/resources/GridResource.h"
#include "server/new/resources/TimeResource.h"
#include "server/new/EventBus.h"
#include "common/utils/Random.h"
#include <cmath>
class ChasingSystem : public ISystem {
private:
    Signature signature_ {};
    Coordinator& coord_;
    EventBus& eventBus_;
    constexpr static float CHASE_FORCE_MAGNITUDE = 3000.f;
    constexpr static float CHECK_INTERVAL = 1.f;
    constexpr static float BASE_START_CHASE_PROB = 0.3f;
    constexpr static float BASE_GIVE_UP_PROB = 0.1f;
    float checkElapse = 0.f;
public:
    explicit ChasingSystem(Coordinator& coordinator, EventBus& eventBus)
        : coord_(coordinator), eventBus_(eventBus) {
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Chase>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Velocity>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Transform>()), true);
        signature_.set(static_cast<size_t>(Coordinator::getComponentTypeID<Force>()), true);
        coord_.registerSystem(signature_);
        // eventBus_.subscribe<EntityDeathEvent>([this](const EntityDeathEvent& event) {
        //     onEntityDeath(event);
        // });
    }
    void update(float dt) override {
        if (checkElapse < CHECK_INTERVAL) {
            checkElapse += dt;
            return;
        }
        checkElapse = 0.f;
        auto& timeRes = coord_.ctx<TimeResource>();
        float angle = (timeRes.currentTime / MAX_TIME) * 2.0f * 3.14159265f;
        float brightness = 1.f - (std::sin(angle) + 1.0f) / 2.0f;
        float startChaseChance = (0.1f + brightness * 0.8f) * BASE_START_CHASE_PROB;
        float stopChaseChance  = (0.9f - brightness * 0.8f) * BASE_GIVE_UP_PROB;
        auto &grid = coord_.ctx<GridResource>();
        for (auto &cell: grid.cells_) {
            if (!cell.isAOI) continue;
            for (Entity entity: cell.entities) {
                if (!coord_.hasSignature(entity, signature_)) continue;
                auto& chase = coord_.getComponent<Chase>(entity);
                auto& transform = coord_.getComponent<Transform>(entity);
                auto& force = coord_.getComponent<Force>(entity);
                if (chase.isChasing) {
                    if (!coord_.hasComponent<Transform>(chase.entity)) {
                        chase.isChasing = false;
                        continue;
                    }
                    auto& targetTrans = coord_.getComponent<Transform>(chase.entity);
                    float dx = targetTrans.x - transform.x;
                    float dy = targetTrans.y - transform.y;
                    float distSq = dx*dx + dy*dy;
                    if (Random::randFloat(0.f, 1.f) < stopChaseChance) {
                        chase.isChasing = false;
                        continue;
                    }
                    float dist = std::sqrt(distSq);
                    if (dist > 0.1f) {
                        float dirX = dx / dist;
                        float dirY = dy / dist;
                        force.ax += dirX * CHASE_FORCE_MAGNITUDE;
                        force.ay += dirY * CHASE_FORCE_MAGNITUDE;
                    }
                }
                else {
                    if (Random::randFloat(0.f, 1.f) < startChaseChance) {
                        std::optional<Entity> target = findTargetInRadius(entity, transform, chase.chaseRadius, grid);
                        if (target != std::nullopt) {
                            chase.isChasing = true;
                            chase.entity = target.value();
                        }
                    }
                }
            }
        }
    }

private:
    void onEntityDeath(const EntityDeathEvent& event) {
        auto& chasers = coord_.getEntitiesWith(signature_);
        for (Entity chaser : chasers) {
            auto& chase = coord_.getComponent<Chase>(chaser);
            if (chase.isChasing && chase.entity == event.entity) {
                chase.isChasing = false;
            }
        }
    }
    std::optional<Entity> findTargetInRadius(Entity self, const Transform& selfTrans, float radius, GridResource& grid) const {
        std::vector<Entity> candidates;
        float rSq = radius * radius;
        int cellX = static_cast<int>(selfTrans.x / grid.cellWidth_);
        int cellY = static_cast<int>(selfTrans.y / grid.cellHeight_);
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int cx = cellX + i;
                int cy = cellY + j;
                if (!grid.cellExistAt(cx, cy)) continue;
                for (Entity other : grid.cellAt(cx, cy).entities) {
                    if (other == self) continue;
                    // if (coord_.hasComponent<Chase>(other)) continue;
                    if (!coord_.hasComponent<Transform>(other)) continue;
                    auto& otherTrans = coord_.getComponent<Transform>(other);
                    float dx = otherTrans.x - selfTrans.x;
                    float dy = otherTrans.y - selfTrans.y;
                    if (dx*dx + dy*dy <= rSq) {
                        candidates.push_back(other);
                    }
                }
            }
        }
        if (candidates.empty()) return std::nullopt;
        int idx = Random::randInt(0, static_cast<int>(candidates.size()) - 1);
        return candidates[idx];
    }
};

#endif //UNDEROCEAN_CHASINGSYSTEM_H