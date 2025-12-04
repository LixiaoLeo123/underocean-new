//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_ENTITYGENERATIONSYSTEM_H
#define UNDEROCEAN_ENTITYGENERATIONSYSTEM_H
#include "ISystem.h"
#include "common/utils/Random.h"
#include "server/new/Coordinator.h"
#include "server/new/Entity/EntityFactory.h"

class EntityGenerationSystem : public ISystem {
private:
    Coordinator& coord_;
    EntityFactory& entityFactory_;
    int maxEntities_;
    float generationSpeed_;
public:
    explicit EntityGenerationSystem(Coordinator& coordinator, EntityFactory& entityFactory, int maxEntities = 0, float generationSpeed = 1.f)
        :coord_(coordinator), entityFactory_(entityFactory), maxEntities_(maxEntities), generationSpeed_(generationSpeed) {
    }
    void setGenerationSpeed(float speed) { generationSpeed_ = speed; }
    void update(float dt) override {
        int missingEntities = maxEntities_ - coord_.getEntityCount();
        if (missingEntities > 0) {
            if (Random::randFloat(0.f, 1.f) <
                static_cast<float>(missingEntities) / static_cast<float>(maxEntities_) * generationSpeed_ * dt) {
                entityFactory_.spawnRandom();
            }
        }
    }
};
#endif //UNDEROCEAN_ENTITYGENERATIONSYSTEM_H