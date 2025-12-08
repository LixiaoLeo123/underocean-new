//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_ENTITYFACTORY_H
#define UNDEROCEAN_ENTITYFACTORY_H
#include <functional>
#include <random>
#include <SFML/System/Vector2.hpp>

#include "server/new/Coordinator.h"
class EntityFactory {
private:
    struct WeightedEntry {
        EntityTypeID type;
        double weight;
    };
    std::mt19937 gen_{ std::random_device{}() };  //for spawnRandom
    Coordinator& coord_;
    sf::Vector2f spawnAreaFrom_ {};
    sf::Vector2f spawnAreaTo_ {};   //spawn area
    std::vector<std::function<Entity(bool isPlayer)>> spawnFunctions_;  //spawn and get Entity value
    std::vector<WeightedEntry> weightedEntries_ {};
    std::discrete_distribution<size_t> dist;
    // bool spawnFunctionsEmpty{ true };  // prevent spawnRandom before initialize
    bool distDirty_{ false };  //see spawnRandom
    void registerSpawner(EntityTypeID id, std::function<Entity(bool isPlayer)> func) {        //spawnFunctions_.push_back
        auto index = static_cast<size_t>(id);
        if (index >= spawnFunctions_.size()) {
            spawnFunctions_.resize(index + 1);
        }
        spawnFunctions_[index] = std::move(func);
    }
public:
    explicit EntityFactory(Coordinator& coordinator)
        :coord_(coordinator) {
        spawnFunctions_.reserve(static_cast<size_t>(EntityTypeID::COUNT));
    };
    void initialize();   //add spawn functions
    void setSpawnArea(sf::Vector2f from, sf::Vector2f to) {
        spawnAreaFrom_ = from;
        spawnAreaTo_ = to;
    }
    Entity spawnWithID(EntityTypeID id, bool isPlayer = false) {
        auto index = static_cast<size_t>(id);
        assert(index < spawnFunctions_.size() && "EntityTypeID not registered!");
        assert(spawnFunctions_[index] && "Spawner function is null!");
        return spawnFunctions_[index](isPlayer);
    }
    void addWeightedEntry(EntityTypeID id, double weight) {
        weightedEntries_.push_back({id, weight});
        distDirty_ = true;
    }
    Entity spawnRandom(bool isPlayer = false) {   //with weight in weightedEntries
        if (distDirty_) {   //after weightedEntries change
            std::vector<double> weights;
            weights.reserve(weightedEntries_.size());
            for (auto& entry : weightedEntries_) {
                weights.push_back(entry.weight);
            }
            dist = std::discrete_distribution<size_t>(weights.begin(), weights.end());
            distDirty_ = false;
        }
        size_t index = dist(gen_);
        return spawnFunctions_[static_cast<size_t>(weightedEntries_[index].type)](isPlayer);
    }
};
#endif //UNDEROCEAN_ENTITYFACTORY_H
