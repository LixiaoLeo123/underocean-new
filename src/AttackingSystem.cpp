#include "server/new/system/AttackingSystem.h"

#include "common/Types.h"
#include "common/utils/Random.h"

void AttackingSystem::onEntityCollision(const EntityCollisionEvent &event) {
    Entity a = event.e1;
    Entity b = event.e2;
    //check if either entity has Attack component
    bool aCanAttack = coord_.hasComponent<Attack>(a);
    bool bCanAttack = coord_.hasComponent<Attack>(b);
    bool aCanBeAttacked = coord_.hasComponent<HP>(a);
    if (coord_.hasComponent<Invincibility>(a) && coord_.getComponent<Invincibility>(a).cooldown > 0.f) {
        aCanBeAttacked = false;
    }
    bool bCanBeAttacked = coord_.hasComponent<HP>(b);
    if (coord_.hasComponent<Invincibility>(b) && coord_.getComponent<Invincibility>(b).cooldown > 0.f) {
        bCanBeAttacked = false;
    }
    const auto& attackHandler = [&](Entity attacker, Entity defender) {
        if (coord_.hasComponent<Invincibility>(defender)) {
            auto& invincibilityComp = coord_.getComponent<Invincibility>(defender);
            invincibilityComp.cooldown = INVINCIBILITY_DURATION;
        }
        float damage = coord_.getComponent<Attack>(attacker).baseDamage *
            coord_.getComponent<Attack>(attacker).skillScale;
        if (damage == 0.f) return;
        if (coord_.hasComponent<Defence>(defender)) {
            damage *= 1 - coord_.getComponent<Defence>(defender).damageReduction;
        }
        auto& hpComp = coord_.getComponent<HP>(defender);
        hpComp.hp -= damage;
        // -- knockback --
        UVector atodVec = (UVector)coord_.getComponent<Transform>(defender) -
            coord_.getComponent<Transform>(attacker);
        if (atodVec.len2() == 0) {  //random dir knockback
            sf::Vector2f randomVec = Random::randUnitVector();
            atodVec = {randomVec.x, randomVec.y};
        }
        else {
            atodVec.normalize();
        }
        constexpr static float KNOCKBACK_VEL = 50.f;
        coord_.getComponent<Velocity>(defender) += atodVec * KNOCKBACK_VEL;
        if (hpComp.hp < 0.f) {
            eventBus_.publish<EntityDeathEvent>({defender});
        }
        eventBus_.publish<EntityHPChangeEvent>({defender, -damage});
    };
    if (aCanAttack && bCanBeAttacked) {
        attackHandler(a, b);
    }
    if (bCanAttack && aCanBeAttacked) {
        attackHandler(b, a);
    }
}
