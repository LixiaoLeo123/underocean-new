#include "server/new/system/DerivedAttributeSystem.h"

#include "server/new/resources/GridResource.h"

void DerivedAttributeSystem::update(float dt) {  //update fp
    auto& grid = coord_.ctx<GridResource>();
    for (auto& cell : grid.cells_) {
        if (!cell.isAOI) continue;
        for (Entity entity : cell.entities) {
            if (!coord_.hasSignature(entity, fpSig_)) continue;
            FP& fp = coord_.getComponent<FP>(entity);
            fp.fp -= fp.fpDecRate * dt;
            EntityTypeID type = coord_.getComponent<EntityType>(entity).entityID;
            if (fp.fp < 0.f) {
                float newSize = coord_.getComponent<Size>(entity).size - calcSizeStep(type);
                if (newSize < calcInitSize(type) * 1.01f) {  //cannot shrink below init size
                    eventBus_.publish<EntityDeathEvent>({entity});
                    continue;
                } else {
                    coord_.getComponent<Size>(entity).size = newSize;
                    fp.maxFp = calcMaxFP(type, newSize);
                    fp.fp = fp.maxFp * 0.8f;  //restore to 80% of new max fp
                    fp.fpDecRate = calcFPDecRate(type, newSize);
                    coord_.getComponent<Mass>(entity).mass = calcMass(type, newSize);
                    coord_.getComponent<MaxAcceleration>(entity).maxAcceleration = calcMaxAcc(type, newSize);
                    coord_.getComponent<MaxVelocity>(entity).maxVelocity = calcMaxVec(type);
                    float maxHP = calcMaxHP(type, newSize);
                    coord_.getComponent<HP>(entity).maxHp = maxHP;
                    if (coord_.getComponent<HP>(entity).hp > maxHP) {
                        coord_.getComponent<HP>(entity).hp = maxHP;
                    }
                    eventBus_.publish<EntitySizeChangeEvent>({entity, newSize});  //notify size change for sync
                }
            } else if (fp.fp > fp.maxFp) {
                float newSize = coord_.getComponent<Size>(entity).size + calcSizeStep(type);
                coord_.getComponent<Size>(entity).size = newSize;
                fp.maxFp = calcMaxFP(type, newSize);
                fp.fp = fp.maxFp * 0.8f;  //restore to 80% of new max fp
                fp.fpDecRate = calcFPDecRate(type, newSize);
                coord_.getComponent<Mass>(entity).mass = calcMass(type, newSize);
                coord_.getComponent<MaxAcceleration>(entity).maxAcceleration = calcMaxAcc(type, newSize);
                coord_.getComponent<MaxVelocity>(entity).maxVelocity = calcMaxVec(type);
                float maxHP = calcMaxHP(type, newSize);
                coord_.getComponent<HP>(entity).maxHp = maxHP;
                if (coord_.getComponent<HP>(entity).hp > maxHP) {
                    coord_.getComponent<HP>(entity).hp = maxHP;
                }
                eventBus_.publish<EntitySizeChangeEvent>({entity, newSize});  //notify size change for sync
            }
            if (coord_.hasComponent<NetworkPeer>(entity)) {
                eventBus_.publish<ClientCommonPlayerAttributesChangeEvent>({
                    coord_.getComponent<NetworkPeer>(entity).peer,
                {
                    coord_.getComponent<HP>(entity).maxHp,
                    fp.maxFp,
                    coord_.getComponent<MaxVelocity>(entity).maxVelocity,
                    coord_.getComponent<MaxAcceleration>(entity).maxAcceleration
                }
                });
            }
        }
    }
}
