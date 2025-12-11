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
public:
    DerivedAttributeSystem(Coordinator& coord, EventBus& eventBus) : coord_(coord) {
        eventBus.subscribe<AttributedEntityInitEvent>([this](const AttributedEntityInitEvent& event) {
            this->initEntityAttributes(event);
        });
    }
    void update(float dt) override {

    }
    void initEntityAttributes(const AttributedEntityInitEvent& event) const;  //from size to attributes, add components
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
};
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
    if (event.shouldAddFP) {
        float maxFP = calcMaxFP(type, size);
        coord_.addComponent<FP>(event.e, {maxFP, maxFP, calcFPDecRate(type, size)});
    }
}
#undef ATTRIBUTE_ENTITY_TYPES
#endif //UNDEROCEAN_DERIVEDATTRIBUTESYSTEM_H