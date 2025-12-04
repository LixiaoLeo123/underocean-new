//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVELBASE_H
#define UNDEROCEAN_LEVELBASE_H
#include "Coordinator.h"
#include "ILevel.h"
#include "Entity/EntityFactory.h"
#include "system/AccelerationLimitSystem.h"
#include "system/AccelerationSystem.h"
#include "system/GridBuildSystem.h"
#include "system/ISystem.h"
#include "system/MovementSystem.h"
#include "system/VelocityLimitSystem.h"

class LevelBase : public ILevel {   //impl basic ecs, abstract
public:
    explicit LevelBase(GameServer& server) : entityFactory_(coordinator_), server_(server) {};
    virtual void initialize() {
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
        emplaceSystem<GridBuildSystem>(coordinator_);
        coordinator_.emplaceContext<GridResource>();
    }
    void finalInitialize() {   //something that must be done after others are all ok
        emplaceSystem<AccelerationLimitSystem>(coordinator_);
        emplaceSystem<AccelerationSystem>(coordinator_);
        emplaceSystem<VelocityLimitSystem>(coordinator_);
        emplaceSystem<MovementSystem>(coordinator_);
    }
    static size_t getUniqueSystemID() {
        static size_t lastID = 0;
        return lastID++;
    }
    template<typename T>
    static size_t getSystemID() {
        static size_t typeID = getUniqueSystemID();
        return typeID;
    }
protected:
    Coordinator coordinator_ {};
    EntityFactory entityFactory_;
    std::vector<std::unique_ptr<ISystem>> systems_ {};
    GameServer& server_;
    virtual void customInitialize() = 0;
    template<typename T, typename... Args>
    void emplaceSystem(Args&&... args) {   //order matters!
        size_t type = getSystemID<T>();
        if (type >= systems_.size()) {
            systems_.resize(type + 1);
        }
        if (!systems_[type]) {
            systems_[type] = std::make_unique<T>(std::forward<Args>(args)...);
        }
    }
    template<typename T>
    T& getSystem() {
        return systems_[getSystemID<T>()];
    }
};
#endif //UNDEROCEAN_LEVELBASE_H