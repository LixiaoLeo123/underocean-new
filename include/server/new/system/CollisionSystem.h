//
// Created by 15201 on 12/17/2025.
//

#ifndef UNDEROCEAN_COLLISIONSYSTEM_H
#define UNDEROCEAN_COLLISIONSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"
#include "server/new/Coordinator.h"
#include "server/new/component/Components.h"
#include <span>
#include <type_traits>
#include <array>
class EventBus;
template <EntityTypeID ID>
constexpr bool has_hit_boxes = requires {
    { ParamTable<ID>::HIT_BOXES } -> std::convertible_to<std::span<const HitBox>>;
};
template <std::size_t... I>
constexpr auto makeHitBoxLookup(std::index_sequence<I...>) {
    return std::array<std::span<const HitBox>,
        static_cast<std::size_t>(EntityTypeID::COUNT)>{
            []<EntityTypeID ID>() {
                if constexpr (has_hit_boxes<ID>) {
                    return std::span<const HitBox>{ ParamTable<ID>::HIT_BOXES };
                } else {
                    return std::span<const HitBox>{};
                }
            }.template operator()<static_cast<EntityTypeID>(I)>()...
        };
}
inline constexpr auto HIT_BOX_LOOKUP =
    makeHitBoxLookup(
        std::make_index_sequence<
            static_cast<std::size_t>(EntityTypeID::COUNT)
        >{}
    );
class CollisionSystem : public ISystem {
private:
    Signature collisionSig_;
    Coordinator& coord_;
    EventBus& eventBus_;
public:
    explicit CollisionSystem(Coordinator& coord, EventBus& eventBus) : coord_(coord), eventBus_(eventBus) {
        {
            collisionSig_.set(Coordinator::getComponentTypeID<Collision>());
            collisionSig_.set(Coordinator::getComponentTypeID<Size>());
            collisionSig_.set(Coordinator::getComponentTypeID<Transform>());
            collisionSig_.set(Coordinator::getComponentTypeID<EntityType>());  //to get default hitbox(es)
            collisionSig_.set(Coordinator::getComponentTypeID<Velocity>());  //to get hitbox rotation
        }
    }
    void update(float dt) override;
    static constexpr std::span<const HitBox> getOriginalHitBoxes(EntityTypeID type) {
        auto idx = static_cast<std::size_t>(type);
        return idx < HIT_BOX_LOOKUP.size() ? HIT_BOX_LOOKUP[idx] : std::span<const HitBox>{};
    }
};
#endif //UNDEROCEAN_COLLISIONSYSTEM_H