#include "server/new/system/SkillSystem.h"

#include "server/GameServer.h"
#include "server/new/EventBus.h"
#include "server/new/component/Components.h"
SkillSystem::SkillSystem(GameServer& server, Coordinator& coord, EventBus& eventBus)
        : server_(server), coord_(coord), eventBus_(eventBus) {
    {
        playerSig_.set(Coordinator::getComponentTypeID<NetworkPeer>(), true);
        coord_.registerSystem(playerSig_);
    }
    skillApplyHandler_.resize(48);  //6 * 8
    skillApplyHandler_[2] = [&](ENetPeer* peer, int level) {  //Small Yellow Dash
        float dashVel = 20.f + level * 9.f;
        eventBus_.publish<PlayerDashEvent>({peer, dashVel});
    };
    skillApplyHandler_[42] = [&](ENetPeer* peer, int level) {  //Small Yellow Dash
        eventBus_.publish<PlayerDashEvent>({peer, dashVel});
    };
    {
        eventBus_.subscribe<PlayerJoinEvent>([this](const PlayerJoinEvent& event) {
            onPlayerJoin(event.playerData.peer);
        });
        eventBus_.subscribe<PlayerLeaveEvent>([this](const PlayerLeaveEvent& event) {
            onPlayerLeave(event.peer);
        });
    }
}
void SkillSystem::update(float dt) {
    //cooldown update
    for (auto& [peer, cooldownData] : skillCooldowns_) {
        for (int i = 0; i < 4; ++i) {
            float& cd = cooldownData.second[i];
            if (cd > 0.f) {
                cd -= dt;
                if (cd < 0.f) {
                    //emit skill ready event
                    if (cooldownData.first) {
                        eventBus_.publish<SkillEndEvent>({peer, i});
                        cooldownData.first = false;
                        cd = getSkillCooldown(getSkillIndices(server_.playerList_[peer].type).skillIndices[i],
                                              server_.playerList_[peer].skillLevels[i]);
                    }
                    else {
                        eventBus_.publish<SkillReadyEvent>({peer, i});
                        cd = -1.f;  //ready
                    }
                }
            }
        }
    }
    auto& networkEntities = coord_.getEntitiesWith(playerSig_);
    for (Entity e : networkEntities) {
        ENetPeer* peer = coord_.getComponent<NetworkPeer>(e).peer;
        auto& actionPackets = server_.buffer_[peer];
        while (!actionPackets.empty()) {
            auto& packet = actionPackets.front();
            PacketReader reader(std::move(*packet));
            std::uint8_t relativeSkillIndex = reader.nextInt8();
            if (relativeSkillIndex > 3) {
                actionPackets.pop();
                continue;   //bad packet
            }
            const auto& it = skillCooldowns_.find(peer);
            if (it == skillCooldowns_.end()) {
                actionPackets.pop();
                continue;   //should not happen
            }
            auto& cooldownData = it->second;
            if (cooldownData.first) {  //skill is applying
                if (cooldownData.second[relativeSkillIndex] > 0.f) {
                    actionPackets.pop();
                    continue;   //still in applying cooldown
                }
                //here handle permanent skill duration, apply when applying then stop
                cooldownData.first = false;  //mark as not applying
                cooldownData.second[relativeSkillIndex] = getSkillCooldown(
                        getSkillIndices(server_.playerList_[peer].type).skillIndices[relativeSkillIndex],
                        server_.playerList_[peer].skillLevels[relativeSkillIndex]);
                eventBus_.publish<SkillEndEvent>({peer, relativeSkillIndex});
                actionPackets.pop();
                continue;
            }
            float& cd = cooldownData.second[relativeSkillIndex];
            if (cd > 0.f) {
                actionPackets.pop();
                continue;   //still in cooldown
            }
            //find skill indices
            SkillIndices skillIndices = getSkillIndices(coord_.getComponent<EntityType>(e).entityID);
            int skillID = skillIndices.skillIndices[relativeSkillIndex];
            //emit skill use event
            eventBus_.publish<SkillApplyEvent>({peer, relativeSkillIndex});
            //assert(skillApplyHandler_[skillID] != nullptr && "Skill apply handler not registered!");
            cooldownData.first = true;  //mark as applying
            cd = getSkillDuration(skillID, server_.playerList_[peer].skillLevels[relativeSkillIndex]);
            if (skillApplyHandler_[skillID]) {
                skillApplyHandler_[skillID](peer, server_.playerList_[peer].skillLevels[relativeSkillIndex]);
            }
            actionPackets.pop();
        }
    }
}
