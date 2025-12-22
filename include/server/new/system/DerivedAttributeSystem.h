//
// Created by 15201 on 12/11/2025.
//

#ifndef UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H
#define UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "common/utils/Random.h"
#include "server/new/Coordinator.h"
#include "server/new/EventBus.h"
#include "server/new/component/Components.h"
//entities that will handle
#define ATTRIBUTE_ENTITY_TYPES \
X(SMALL_YELLOW)

class DerivedAttributeSystem : public ISystem{  //calculate maxHP, maxFP, etc. by entity size and type
private:
    Coordinator& coord_;
    EventBus& eventBus_;  //to publish entity death event
    Signature fpSig_;
public:
    DerivedAttributeSystem(Coordinator& coord, EventBus& eventBus) : coord_(coord), eventBus_(eventBus) {
        eventBus.subscribe<AttributedEntityInitEvent>([this](const AttributedEntityInitEvent& event) {
            this->initEntityAttributes(event);
        });
        eventBus.subscribe<PlayerRespawnEvent>([this](const PlayerRespawnEvent& event) {  //reset attributes
            this->onPlayerRespawn(event);
        });
        eventBus.subscribe<EntityCollisionEvent>([this](const EntityCollisionEvent& event) {
            this->onEntityCollision(event);
        });
        eventBus.subscribe<EntityDeathEvent>([this](const EntityDeathEvent& event) {
            this->onEntityDeath(event);
        }, 1);
        fpSig_.set(Coordinator::getComponentTypeID<FP>(), true);
        fpSig_.set(Coordinator::getComponentTypeID<HP>(), true);
        fpSig_.set(Coordinator::getComponentTypeID<Size>(), true);
        fpSig_.set(Coordinator::getComponentTypeID<Mass>(), true);
        fpSig_.set(Coordinator::getComponentTypeID<MaxVelocity>(), true);
        fpSig_.set(Coordinator::getComponentTypeID<MaxAcceleration>(), true);
    }
    void update(float dt) override;  //update fp
    void onPlayerRespawn(const PlayerRespawnEvent& event) const;  //reset attributes
    void onEntityCollision(const EntityCollisionEvent& event) const;  //check if it is FoodBall, if then add FP
    void onEntityDeath(const EntityDeathEvent& event) const;  //if entity with FP dies, drop FP
    void initEntityAttributes(const AttributedEntityInitEvent& event) const;  //from size to attributes, add components
    static float calcAttackDamage(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::ATTACK_DAMAGE_BASE * size * size \
            / ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcMaxHP(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::HP_BASE / ParamTable<EntityTypeID::type>::INIT_SIZE * size; break;
        ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcMaxFP(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::FP_BASE * size * size \
            / ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcMass(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::MASS_BASE * size * size \
            / ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcFPDecRate(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::FP_DEC_RATE_BASE * size * size * size\
/ ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcMaxAcc(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::MAX_FORCE / (ParamTable<EntityTypeID::type>::MASS_BASE * size * size \
            / ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE); break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcMaxVec(EntityTypeID type) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::MAX_VELOCITY; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcInitSize(EntityTypeID type) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::INIT_SIZE; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcSizeStep(EntityTypeID type) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::SIZE_STEP; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return -1.f;
        }
    }
    static float calcNutrition(EntityTypeID type, float size) {
        switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::BASE_NUTRITION * size * size \
            / ParamTable<EntityTypeID::type>::INIT_SIZE / ParamTable<EntityTypeID::type>::INIT_SIZE; break;
            ATTRIBUTE_ENTITY_TYPES
#undef X
            default: return 0.f;
        }
    }
};
inline void DerivedAttributeSystem::onPlayerRespawn(const PlayerRespawnEvent &event) const {
    Entity e = event.player;
    float size = coord_.getComponent<Size>(e).size;
    EntityTypeID type = coord_.getComponent<EntityType>(e).entityID;
    {  //max acceleration
        coord_.getComponent<MaxAcceleration>(e).maxAcceleration = calcMaxAcc(type, size);
    }
    {  //mass
        coord_.getComponent<Mass>(e).mass = calcMass(type, size);
    }
    {  //hp
        float maxHP = calcMaxHP(type, size);
        HP& hpComp =  coord_.getComponent<HP>(e);
        hpComp.maxHp = maxHP;
        hpComp.hp = maxHP;
    }
    if (coord_.hasComponent<Attack>(e)) {  //attack
        coord_.getComponent<Attack>(e).baseDamage = calcAttackDamage(type, size);
    }
    if (coord_.hasComponent<FP>(e)) {
        float maxFP = calcMaxFP(type, size);
        FP& fpComp = coord_.getComponent<FP>(e);
        fpComp.maxFp = maxFP;
        fpComp.fp = maxFP;
        fpComp.fpDecRate = calcFPDecRate(type, size);
    }
    eventBus_.publish<ClientCommonPlayerAttributesChangeEvent>({
        coord_.getComponent<NetworkPeer>(e).peer,
{
        coord_.getComponent<HP>(e).maxHp,
        coord_.getComponent<FP>(e).maxFp,
        coord_.getComponent<MaxVelocity>(e).maxVelocity,
        coord_.getComponent<MaxAcceleration>(e).maxAcceleration
        }
    });
}
inline void DerivedAttributeSystem::onEntityDeath(const EntityDeathEvent &event) const {
    auto& transform = coord_.getComponent<Transform>(event.entity);
    EntityTypeID type = coord_.getComponent<EntityType>(event.entity).entityID;
    float size = coord_.getComponent<Size>(event.entity).size;
    Entity foodBall = coord_.createEntity();
    coord_.addComponent<Transform>(foodBall, transform);
    coord_.addComponent(foodBall, Velocity());
    coord_.addComponent<Size>(foodBall, {1.f});
    coord_.addComponent<FoodBall>(foodBall, {calcNutrition(type, size)});
    coord_.addComponent<EntityType>(foodBall, {EntityTypeID::FOOD_BALL});
    coord_.addComponent<Collision>(foodBall, {});
    coord_.addComponent<NetSyncComp>(foodBall, {
        static_cast<std::uint8_t>(Random::randInt(0, TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC - 1))
        });
    coord_.notifyEntityChanged(foodBall);
}
inline void DerivedAttributeSystem::onEntityCollision(const EntityCollisionEvent &event) const {
    const auto& handleFPIncrease = [this](Entity e1, Entity e2) {
        if (coord_.hasComponent<FoodBall>(e1) && coord_.hasComponent<FP>(e2)) {
            auto& fpComp = coord_.getComponent<FP>(e2);
            auto& foodComp = coord_.getComponent<FoodBall>(e1);
            fpComp.fp += foodComp.nutrition;
            coord_.addComponent<EntityClearTag>(e1, {});
            coord_.notifyEntityChanged(e1);
        }
    };
    handleFPIncrease(event.e1, event.e2);
    handleFPIncrease(event.e2, event.e1);
}
inline void DerivedAttributeSystem::initEntityAttributes(const AttributedEntityInitEvent& event) const{  //should ensure size and type exists, fp for players
    float size = coord_.getComponent<Size>(event.e).size;
    EntityTypeID type = coord_.getComponent<EntityType>(event.e).entityID;
    {  //max acceleration
        coord_.addComponent<MaxAcceleration>(event.e, {calcMaxAcc(type, size)});
    }
    {  //mass
        coord_.addComponent<Mass>(event.e, {calcMass(type, size)});
    }
    {  //hp
        float maxHP = calcMaxHP(type, size);
        coord_.addComponent<HP>(event.e, {maxHP, maxHP});
    }
    if (event.shouldAddAttack) {  //attack
        coord_.addComponent<Attack>(event.e, {calcAttackDamage(type, size),
            event.isPlayer? 0.f : 1.f});
    }
    if (event.shouldAddFP) {
        float maxFP = calcMaxFP(type, size);
        coord_.addComponent<FP>(event.e, {maxFP, maxFP, calcFPDecRate(type, size)});
    }
    coord_.notifyEntityChanged(event.e);
}
#undef ATTRIBUTE_ENTITY_TYPES
#endif //UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H