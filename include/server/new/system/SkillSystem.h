#ifndef UNDEROCEAN_SKILLSYSTEM_H
#define UNDEROCEAN_SKILLSYSTEM_H
#include <functional>
#include <array> // 记得包含这个
#include "ISystem.h"
#include "common/Types.h"
#include "server/GameServer.h"
#include "server/new/Coordinator.h"

class EventBus;
struct NetworkPeer;
class GameServer;

class SkillSystem : public ISystem {
public:
    explicit SkillSystem(GameServer& server, Coordinator& coord, EventBus& eventBus);
    void update(float dt) override;
    constexpr static SkillIndices getSkillIndices(EntityTypeID type);
    constexpr static float getSkillCooldown(int skillIndex, int level);
    constexpr static float getSkillDuration(int skillIndex, int level);
private:
    constexpr static int TOTAL_SKILL_NUM = 48;
    Signature playerSig_ {};
    GameServer& server_;
    Coordinator& coord_;
    EventBus& eventBus_;
    struct SkillState {
        bool isActive = false;
        float timer = 0.f;
    };
    std::unordered_map<ENetPeer*, std::array<SkillState, 4>> skillStates_;

    std::vector<std::function<void(ENetPeer* peer, int level)>> skillApplyHandler_ {};
    std::vector<std::function<void(ENetPeer* peer, int level)>> skillEndHandler_ {};
    void onPlayerJoin(ENetPeer* peer);
    void onPlayerLeave(ENetPeer* peer);
};

constexpr float SkillSystem::getSkillCooldown(int skillIndex, int level) {
    switch (skillIndex) {
        case 42:  //attack enabled
            return 1.f;
        case 2: //Small Yellow Dash
            return 10.f - level * 1.f;
        case 24:
            return 45.f - level * 2.f;
        case 1:
            return 60.f - level * 7.f;
        case 9:
            return 50.f - level * 3.f;
        case 17:
            return 30.f - level * 4.f;
        case 13:
            return 25.f - level * 5.f;
         default:
            return 1.f;
    }
}
constexpr float SkillSystem::getSkillDuration(int skillIndex, int level) {
    switch (skillIndex) {
        case 42:  //attack enabled
            return -1.f;  //permanent
        case 2: //Small Yellow Dash
            return 0.5f;
        case 24:
            return 4.f + level * 2.f;
        case 1:
            return 20.f + level * 5.f;
        case 9:
            return 5.f + level * 2.f;
        case 17:
            return 1.f;
        case 13:
            return 10.f * level * 5.f;
        default:
            return -1.f;
    }
}
constexpr SkillIndices SkillSystem::getSkillIndices(EntityTypeID type) {
    switch (type) {
#define X(type) case EntityTypeID::type: return ParamTable<EntityTypeID::type>::SKILL_INDICES;
        PLAYER_ENTITY_TYPES
#undef X
        default:
            return {0, 0, 0, 0};
    }
}
#endif //UNDEROCEAN_SKILLSYSTEM_H
