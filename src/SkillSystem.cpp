#include "server/new/system/SkillSystem.h"
#include "server/GameServer.h"
#include "server/new/EventBus.h"
#include "server/new/component/Components.h"
#include "server/new/system/DerivedAttributeSystem.h"

SkillSystem::SkillSystem(GameServer& server, Coordinator& coord, EventBus& eventBus)
        : server_(server), coord_(coord), eventBus_(eventBus) {
    {
        playerSig_.set(Coordinator::getComponentTypeID<NetworkPeer>(), true);
        coord_.registerSystem(playerSig_);
    }
    skillApplyHandler_.resize(TOTAL_SKILL_NUM);
    skillEndHandler_.resize(TOTAL_SKILL_NUM);
    // Small Yellow Dash
    skillApplyHandler_[2] = [&](ENetPeer* peer, int level) {
        float dashVel = 20.f + level * 9.f;
        eventBus_.publish<PlayerDashEvent>({peer, dashVel});
    };
    {  //Attack
        skillApplyHandler_[42] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    assert(coord_.hasComponent<Attack>(e));
                    coord_.getComponent<Attack>(e).skillScale = 1.f + 0.2f * level;
                    break;
                }
            }
        };
        skillEndHandler_[42] = [&](ENetPeer* peer, int level) {  //Attack
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    assert(coord_.hasComponent<Attack>(e));
                    coord_.getComponent<Attack>(e).skillScale = 0.f;
                    break;
                }
            }
        };
    }
    {  //regeneration
        skillApplyHandler_[24] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    assert(coord_.hasComponent<HP>(e));
                    coord_.getComponent<HP>(e).hpIncRate = 0.5f + level * 0.5f;
                    break;
                }
            }
        };
        skillEndHandler_[24] = [&](ENetPeer* peer, int level) {  //Attack
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    assert(coord_.hasComponent<HP>(e));
                    coord_.getComponent<HP>(e).hpIncRate = 0.f;
                    break;
                }
            }
        };
    }
    {  //glow
        skillApplyHandler_[1] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    eventBus_.publish<PlayerGlowSetEvent>({e, 2.5f + level * 1.2f,
                        static_cast<std::uint8_t>(155u + level * 24u)});
                    break;
                }
            }
        };
        skillEndHandler_[1] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    eventBus_.publish<PlayerGlowSetEvent>({e, 0.f,
                        static_cast<std::uint8_t>(0)});
                    break;
                }
            }
        };
    }
    {  //speedup
        skillApplyHandler_[9] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    eventBus_.publish<ClientCommonPlayerAttributesChangeEvent>({
                        coord_.getComponent<NetworkPeer>(e).peer,
                        {
                            coord_.getComponent<HP>(e).maxHp,
                            coord_.getComponent<FP>(e).maxFp,
                            DerivedAttributeSystem::calcMaxVec(coord_.getComponent<EntityType>(e).entityID) * (1.3f + level * 0.5f),
                            coord_.getComponent<MaxAcceleration>(e).maxAcceleration
                        }
                    });
                    break;
                }
            }
        };
        skillEndHandler_[9] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    eventBus_.publish<ClientCommonPlayerAttributesChangeEvent>({
                        coord_.getComponent<NetworkPeer>(e).peer,
                        {
                            coord_.getComponent<HP>(e).maxHp,
                            coord_.getComponent<FP>(e).maxFp,
                            DerivedAttributeSystem::calcMaxVec(coord_.getComponent<EntityType>(e).entityID),
                            coord_.getComponent<MaxAcceleration>(e).maxAcceleration
                        }
                    });
                    break;
                }
            }
        };
    }
    {  //dash (with invincibility)
        skillApplyHandler_[17] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    float dashVel = 20.f + level * 9.f;
                    eventBus_.publish<PlayerDashEvent>({peer, dashVel});
                    coord_.addComponent<Invincibility>(e, {1.f});
                    break;
                }
            }
        };
    }
    {  //fp up
        skillApplyHandler_[13] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    coord_.getComponent<FP>(e).fpDecRate = -level * 0.1f;
                    break;
                }
            }
        };
        skillEndHandler_[13] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    coord_.getComponent<FP>(e).fpDecRate = DerivedAttributeSystem::calcFPDecRate(coord_.getComponent<EntityType>(e).entityID,
                        coord_.getComponent<Size>(e).size);
                    break;
                }
            }
        };
    }
    {  //invincibility
        skillApplyHandler_[35] = [&](ENetPeer* peer, int level) {
            auto& peerEntities = coord_.getEntitiesWith(playerSig_);
            for (Entity e : peerEntities) {
                if (coord_.getComponent<NetworkPeer>(e).peer == peer) {
                    coord_.addComponent<Invincibility>(e, {3.f + level * 1.f});
                    coord_.notifyEntityChanged(e);
                    break;
                }
            }
        };
    }
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
    for (auto& [peer, statesArray] : skillStates_) {
        SkillIndices indices = getSkillIndices(server_.playerList_[peer].type);
        for (int i = 0; i < 4; ++i) {
            SkillState& state = statesArray[i];
            if (state.timer > 0.f) {
                state.timer -= dt;
                if (state.timer <= 0.f) {
                    int skillID = indices.skillIndices[i];
                    int level = server_.playerList_[peer].skillLevels[i];
                    if (state.isActive) {
                        if (skillEndHandler_[skillID]) {
                            skillEndHandler_[skillID](peer, level);
                        }
                        eventBus_.publish<SkillEndEvent>({peer, i});
                        state.isActive = false;
                        state.timer = getSkillCooldown(skillID, level);
                    } else {
                        eventBus_.publish<SkillReadyEvent>({peer, i});
                        state.timer = 0.f;
                    }
                }
            }
        }
    }
    auto& networkEntities = coord_.getEntitiesWith(playerSig_);
    for (Entity e : networkEntities) {
        ENetPeer* peer = coord_.getComponent<NetworkPeer>(e).peer;
        auto& actionPackets = server_.buffer_[peer];
        if (skillStates_.find(peer) == skillStates_.end()) continue;
        while (!actionPackets.empty()) {
            auto& packet = actionPackets.front();
            PacketReader reader(std::move(*packet));
            std::uint8_t relativeSkillIndex = reader.nextInt8();
            if (relativeSkillIndex > 3) {
                actionPackets.pop();
                continue;
            }
            SkillState& state = skillStates_[peer][relativeSkillIndex];
            SkillIndices indices = getSkillIndices(server_.playerList_[peer].type);
            int skillID = indices.skillIndices[relativeSkillIndex];
            int level = server_.playerList_[peer].skillLevels[relativeSkillIndex];
            if (state.isActive) {
                float durationProps = getSkillDuration(skillID, level);
                if (durationProps < 0.f) {
                    state.isActive = false;
                    if (skillEndHandler_[skillID]) {
                        skillEndHandler_[skillID](peer, level);
                    }
                    eventBus_.publish<SkillEndEvent>({peer, relativeSkillIndex});
                    state.timer = getSkillCooldown(skillID, level);
                }
            }
            else {
                if (state.timer <= 0.f) {
                    state.isActive = true;
                    eventBus_.publish<SkillApplyEvent>({peer, relativeSkillIndex});
                    if (skillApplyHandler_[skillID]) {
                        skillApplyHandler_[skillID](peer, level);
                    }
                    state.timer = getSkillDuration(skillID, level);
                }
            }
            actionPackets.pop();
        }
    }
}
void SkillSystem::onPlayerJoin(ENetPeer *peer) {
    if (skillStates_.find(peer) != skillStates_.end()) return;
    SkillIndices indices = getSkillIndices(server_.playerList_[peer].type);
    std::array<SkillState, 4> initialStates;
    for (int i = 0; i < 4; ++i) {
        initialStates[i].isActive = false;
        initialStates[i].timer = getSkillCooldown(indices.skillIndices[i], server_.playerList_[peer].skillLevels[i]);
    }
    skillStates_.emplace(peer, initialStates);
}
void SkillSystem::onPlayerLeave(ENetPeer *peer) {
    skillStates_.erase(peer);
}