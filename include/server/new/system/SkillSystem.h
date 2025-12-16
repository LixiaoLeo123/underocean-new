//
// Created by 15201 on 12/15/2025.
//

#ifndef UNDEROCEAN_SKILLSYSTEM_H
#define UNDEROCEAN_SKILLSYSTEM_H
#include <functional>

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
    Signature playerSig_ {};
    GameServer& server_;   //to get buffered action data
    Coordinator& coord_;  //to get entity and modify entity
    EventBus& eventBus_;  //to publish skill emit, subscribe player join and leave event
    std::unordered_map<ENetPeer*, std::pair<bool, std::array<float, 4>>> skillCooldowns_ {};  //peer to skill cooldowns, bool for enabled, cooldown for both skill ready cooldown and skill end cooldown
    std::vector<std::function<void(ENetPeer* peer, int level)>> skillApplyHandler_ {};
    void onPlayerJoin(ENetPeer* peer);
    void onPlayerLeave(ENetPeer* peer);
};
inline void SkillSystem::onPlayerJoin(ENetPeer *peer) {
    const auto& it = skillCooldowns_.find(peer);
    if (it != skillCooldowns_.end()) {
        return; //already exist
    }
    SkillIndices indices = getSkillIndices(server_.playerList_[peer].type);
    float cooldowns[4];
    for (int i = 0; i < 4; ++i) {
        cooldowns[i] = getSkillCooldown(indices.skillIndices[i], server_.playerList_[peer].skillLevels[i]);
    }
    skillCooldowns_.emplace(peer, std::make_pair(false, std::array<float, 4>{cooldowns[0], cooldowns[1], cooldowns[2], cooldowns[3]}));
}

inline void SkillSystem::onPlayerLeave(ENetPeer *peer) {
    skillCooldowns_.erase(peer);
}
constexpr float SkillSystem::getSkillCooldown(int skillIndex, int level) {
    switch (skillIndex) {
        case 42:  //attack enabled
            return 1.f;
        case 2: //Small Yellow Dash
            return 10.f - level * 1.f;
        default:
            return 0.f;
    }
}
constexpr float SkillSystem::getSkillDuration(int skillIndex, int level) {
    switch (skillIndex) {
        case 42:  //attack enabled
            return -1.f;  //permanent
        case 2: //Small Yellow Dash
            return 0.5f;
        default:
            return 0.f;
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
