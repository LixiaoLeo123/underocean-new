//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_COMPONENTARRAY_H
#define UNDEROCEAN_COMPONENTARRAY_H
#include <unordered_map>
#include "IComponentArray.h"
template<typename T>
class ComponentArray : public IComponentArray {
private:
    std::array<T, MAX_ENTITIES> componentArray;
    std::unordered_map<Entity, size_t> entityToIndexMap;
    std::unordered_map<size_t, Entity> indexToEntityMap;
    size_t size = 0;
public:
    void InsertData(Entity entity, T component) {
        size_t newIndex = size;
        entityToIndexMap[entity] = newIndex;
        indexToEntityMap[newIndex] = entity;
        componentArray[newIndex] = component;
        ++size;
    }
    void RemoveData(Entity entity) {   //remove the last one, unsafe when not found, use EntityDestroyed
        size_t removedEntityIndex = entityToIndexMap[entity];
        size_t lastElementIndex = size - 1;
        T lastElement = componentArray[lastElementIndex];
        Entity lastEntity = indexToEntityMap[lastElementIndex];
        componentArray[removedEntityIndex] = lastElement;
        entityToIndexMap[lastEntity] = removedEntityIndex;
        indexToEntityMap[removedEntityIndex] = lastEntity;
        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(lastElementIndex);
        --size;
    }
    T& GetData(Entity entity) { //unsafe
        return componentArray[entityToIndexMap[entity]];
    }
    void EntityDestroyed(Entity entity) override {
        if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
            RemoveData(entity);
        }
    }
};
#endif