//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_LEVELBASE_H
#define UNDEROCEAN_LEVELBASE_H
#include "Coordinator.h"
#include "EventBus.h"
#include "ILevel.h"
#include "Entity/EntityFactory.h"
#include "system/ISystem.h"
#include "system/MovementSystem.h"
#include "server/GameServer.h"
class LevelBase : public ILevel {   //impl basic ecs, abstract
public:
    explicit LevelBase(GameServer& server) : entityFactory_(coordinator_), server_(server) {
        entityFactory_.initialize(eventBus_);
    };
    virtual void initialize() = 0;
    void update(float dt) override {
        for (auto& system : systems_) {
            system->update(dt);
        }
        ++currentTick;
    }
    [[nodiscard]] unsigned getCurrentTick() const { return currentTick; }
    virtual UVector getMapSize() = 0;
    virtual std::uint16_t ltonX(float x) = 0;  //local to net x
    virtual float ntolX(std::uint16_t x) = 0;
    virtual std::uint16_t ltonY(float y) = 0;  //local to net y
    virtual float ntolY(std::uint16_t y) = 0;
    virtual int getLevelID() const = 0;
private:
    // void coreInitialize() {
    //     coordinator_.emplaceContext<GridResource>();
    //     emplaceSystem<GridBuildSystem>(coordinator_);
    // }
    // void finalInitialize() {   //something that must be done after others are all ok
    //     emplaceSystem<AccelerationLimitSystem>(coordinator_);
    //     emplaceSystem<AccelerationSystem>(coordinator_);
    //     emplaceSystem<VelocityLimitSystem>(coordinator_);
    //     emplaceSystem<MovementSystem>(coordinator_);
    // }
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
    GameServer& server_;  //for network
    EventBus eventBus_;
    unsigned currentTick { 0u };
    // virtual void customInitialize() = 0;
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
        return *static_cast<T*>(systems_[getSystemID<T>()].get());
    }
};
#endif //UNDEROCEAN_LEVELBASE_H