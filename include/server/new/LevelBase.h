//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVELBASE_H
#define UNDEROCEAN_LEVELBASE_H
#include "Coordinator.h"
#include "ILevel.h"
#include "system/AccelerationLimitSystem.h"
#include "system/AccelerationSystem.h"
#include "system/GridBuildSystem.h"
#include "system/ISystem.h"
#include "system/MovementSystem.h"
#include "system/VelocityLimitSystem.h"

class LevelBase : public ILevel {   //impl basic ecs, abstract
public:
    LevelBase() = default;
    void initialize() {
        coreInitialize();
        customInitialize();
        finalInitialize();
    }
    void update(float dt) override {
        for (auto& system : systems_) {
            system->update(dt);
        }
    }
private:
    void coreInitialize() {
        systems_.emplace_back(std::make_unique<GridBuildSystem>(coordinator_));
        coordinator_.emplaceContext<GridResource>();
    }
    void finalInitialize() {   //something that must be done after others are all ok
        systems_.emplace_back(std::make_unique<AccelerationLimitSystem>(coordinator_));
        systems_.emplace_back(std::make_unique<AccelerationSystem>(coordinator_));
        systems_.emplace_back(std::make_unique<VelocityLimitSystem>(coordinator_));
        systems_.emplace_back(std::make_unique<MovementSystem>(coordinator_));
    }
protected:
    Coordinator coordinator_ {};
    std::vector<std::unique_ptr<ISystem>> systems_ {};
    virtual void customInitialize() = 0;
};
#endif //UNDEROCEAN_LEVELBASE_H