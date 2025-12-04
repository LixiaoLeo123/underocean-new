//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_ENTITYFACTORY_H
#define UNDEROCEAN_ENTITYFACTORY_H
#include <functional>
#include <SFML/System/Vector2.hpp>

#include "server/new/Coordinator.h"
class EntityFactory {
private:
    Coordinator& coord_;
    sf::Vector2f spawnAreaFrom_ {};
    sf::Vector2f spawnAreaTo_ {};   //spawn area
    std::vector<std::function<Entity()>> spawnFunctions_;  //spawn and get Entity value
    void registerSpawner(EntityTypeID id, std::function<Entity()> func) {        //spawnFunctions_.push_back
        auto index = static_cast<size_t>(id);
        if (index >= spawnFunctions_.size()) {
            spawnFunctions_.resize(index + 1);
        }
        spawnFunctions_[index] = std::move(func);
    }
public:
    explicit EntityFactory(Coordinator& coordinator)
        :coord_(coordinator) {
        spawnFunctions_.reserve(static_cast<size_t>(EntityTypeID::NUM));
    };
    void initialize();   //add spawn functions
    void setSpawnArea(sf::Vector2f from, sf::Vector2f to) {
        spawnAreaFrom_ = from;
        spawnAreaTo_ = to;
    }
    Entity spawnWithID(EntityTypeID id) {
        auto index = static_cast<size_t>(id);
        assert(index < spawnFunctions_.size() && "EntityTypeID not registered!");
        assert(spawnFunctions_[index] && "Spawner function is null!");
        return spawnFunctions_[index]();
    }
};
#endif //UNDEROCEAN_ENTITYFACTORY_H
