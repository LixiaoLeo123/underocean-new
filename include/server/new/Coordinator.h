#ifndef UNDEROCEAN_COORDINATOR_H
#define UNDEROCEAN_COORDINATOR_H
#include <array>
#include <cassert>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <any>
#include "common/Types.h"
#include "component/ComponentArray.h"
#include "component/IComponentArray.h"
/*  if a system do use GridResource to tick in a specific range,
 *  DO NOT register its signature here,
 *  instead take entity from GridResource and select in update()
 *  else if this system is global and do not use GridResource,
 *  register to improve performance
 */
class Coordinator {
private:
    std::array<Signature, MAX_ENTITIES> signatures{};
    std::queue<Entity> availableIds;  //from destroyed entities
    Entity nextId = 0;  //fully new id
    std::vector<std::unique_ptr<IComponentArray>> componentArrays;
    std::unordered_map<ResourceType, std::unique_ptr<void, void(*)(void*)>> resources;   //just global data, no logic
    std::unordered_map<Signature, std::vector<Entity>> entitiesBySignature;  //system
    static ComponentType getUniqueComponentTypeID() {
        static ComponentType lastID = 0;
        return lastID++;
    }
    static ResourceType getUniqueResourceTypeID() {
        static ResourceType lastID = 0;
        return lastID++;
    }
    template<typename T>
    ComponentArray<T>* getComponentArray() {
        ComponentType type = getComponentTypeID<T>();
        assert(!(type >= componentArrays.size() || !componentArrays[type]) && "Component not registered!");
        return static_cast<ComponentArray<T>*>(componentArrays[type].get());
    }
    template<typename T>
    void registerComponent() {   //single component can be registered multiple times
        ComponentType type = getComponentTypeID<T>();
        if (type >= componentArrays.size()) {
            componentArrays.resize(type + 1);
        }
        if (!componentArrays[type]) {
            componentArrays[type] = std::make_unique<ComponentArray<T>>();
        }
    }
    template<typename T>
    static void deleteResource(void *ptr) {
        delete static_cast<T *>(ptr);
    }
public:
    Coordinator(const Coordinator&) = delete;
    Coordinator& operator=(const Coordinator&) = delete;
    Coordinator(const Coordinator&&) = delete;
    Coordinator& operator=(const Coordinator&&) = delete;
    Coordinator();
    // static Coordinator& getInstance() {
    //     static Coordinator instance;
    //     return instance;
    // }
    template<typename Component>
    static ComponentType getComponentTypeID() {
        static ComponentType typeID = getUniqueComponentTypeID();
        return typeID;
    }
    template<typename T>
    static ResourceType getResourceTypeID() {
        static ResourceType typeID = getUniqueResourceTypeID();
        return typeID;
    }
    template<typename T, typename... Args>
    void emplaceContext(Args&&... args) {
        ResourceType id = getResourceTypeID<T>();
        T* rawPtr = new T(std::forward<Args>(args)...);
        void (*deleter)(void*) = &deleteResource<T>;
        resources.emplace(id, std::unique_ptr<void, void(*)(void*)>(
            static_cast<void*>(rawPtr),
            deleter
        ));
    }
    template<typename T>
    T& ctx() {    //get resource
        ResourceType id = getResourceTypeID<T>();
        auto it = resources.find(id);
        assert(it != resources.end() && "Resource not set! Did you call setContext<T>()?");
        void* rawPtr = it->second.get();
        return *static_cast<T*>(rawPtr);
    }
    Entity createEntity() {
        Entity id;
        if (!availableIds.empty()) {
            id = availableIds.front();
            availableIds.pop();
        } else {
            if (nextId >= MAX_ENTITIES) {
                throw std::runtime_error("Too many entities!");
            }
            id = nextId++;
        }
        signatures[id].reset();
        return id;
    }
    void destroyEntity(Entity entity) {
        for (auto& array : componentArrays) {
            if (array) {
                array->EntityDestroyed(entity);
            }
        }
        signatures[entity].reset();
        for (auto& pair : entitiesBySignature) {
            std::vector<Entity>& list = pair.second;
            list.erase(std::remove(list.begin(), list.end(), entity), list.end());
        }
        availableIds.push(entity);
    }
    template<typename T>
    void addComponent(Entity entity, T component) {  //must call notifyEntityChanged manually!!
        getComponentArray<T>()->InsertData(entity, component);
        signatures[entity].set(getComponentTypeID<T>(), true);
    }
    template<typename T>
    void removeComponent(Entity entity) {  //must call notifyEntityChanged manually!!
        getComponentArray<T>()->RemoveData(entity);
        signatures[entity].set(getComponentTypeID<T>(), false);
    }
    template<typename T>
    T& getComponent(Entity entity) {  //unsafe
        return getComponentArray<T>()->GetData(entity);
    }
    void registerSystem(Signature signature) {   //call by system only, copy va
        if (entitiesBySignature.find(signature) == entitiesBySignature.end()) {
            entitiesBySignature[signature] = {};   //only init when not exist
        }
    }
    const std::vector<Entity>& getEntitiesWith(Signature signature) {
        return entitiesBySignature[signature];
    }
    void notifyEntityChanged(Entity entity) {
        for (auto& pair : entitiesBySignature) {
            const Signature& sig = pair.first;
            std::vector<Entity>& list = pair.second;
            bool nowMatches = (signatures[entity] & sig) == sig;
            auto it = std::find(list.begin(), list.end(), entity);
            bool wasInList = (it != list.end());
            if (nowMatches && !wasInList) {
                list.push_back(entity);
            } else if (!nowMatches && wasInList) {
                *it = list.back();
                list.pop_back();
            }
        }
    }
    template<typename T>
    [[nodiscard]] bool hasComponent(Entity entity) const {
        ComponentType type = getComponentTypeID<T>();
        return signatures[entity].test(type);
    }
    [[nodiscard]] bool hasSignature(Entity entity, Signature targetSignature) const {
        return (signatures[entity] & targetSignature) == targetSignature;
    }
    [[nodiscard]] int getEntityCount() {
        return static_cast<int>(entitiesBySignature[Signature{}].size());
    }
};
#endif //UNDEROCEAN_COORDINATOR_H