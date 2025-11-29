//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVELBASE_H
#define UNDEROCEAN_LEVELBASE_H
#include "Coordinator.h"
#include "ILevel.h"
#include "system/AccelerationSystem.h"
#include "system/GridBuildSystem.h"
#include "system/ISystem.h"
#include "system/MovementSystem.h"
class Coordinator;
class LevelBase : public ILevel {   //impl basic ecs
public:
    LevelBase() {
        systems_.emplace_back(std::make_unique<GridBuildSystem>(coordinator_));
        systems_.emplace_back(std::make_unique<MovementSystem>(coordinator_));
        systems_.emplace_back(std::make_unique<AccelerationSystem>(coordinator_));
    }
    void update(float dt) override {
        for (auto& system : systems_) {
            system->update(dt);
        }
    }
protected:
    Coordinator coordinator_;
    std::vector<std::unique_ptr<ISystem>> systems_;
};
#endif //UNDEROCEAN_LEVELBASE_H