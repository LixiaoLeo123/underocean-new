#include "server/new/system/NetworkSyncSystem.h"

#include "common/net(depricate)/PacketWriter.h"
#include "common/network/ServerNetworkDriver.h"
#include "server/GameServer.h"
#include "server/new/LevelBase.h"
#include "server/new/resources/GridResource.h"
#include "server/new/resources/TimeResource.h"

NetworkSyncSystem::NetworkSyncSystem(Coordinator &coordinator, GameServer &server, LevelBase &level, EventBus &eventbus)
    : coord_(coordinator), server_(server), level_(level) {
    {
        signature_.set(Coordinator::getComponentTypeID<NetworkPeer>(), true);
        signature_.set(Coordinator::getComponentTypeID<Transform>(), true);
        coord_.registerSystem(signature_);
    }
    eventbus.subscribe<EntitySizeChangeEvent>([this](const EntitySizeChangeEvent &event) {
        this->onEntitySizeChange(event);
    });
    eventbus.subscribe<EntityHPChangeEvent>([this](const EntityHPChangeEvent &event) {
        this->onEntityHPChange(event);
    });
    eventbus.subscribe<PlayerLeaveEvent>([this](const PlayerLeaveEvent &event) {
        this->onPlayerLeave(event);
    });
    eventbus.subscribe<PlayerJoinEvent>([this](const PlayerJoinEvent &event) {
        this->onPlayerJoin(event);
    });
    eventbus.subscribe<ClientCommonPlayerAttributesChangeEvent>(
        [this](const ClientCommonPlayerAttributesChangeEvent &event) {
            this->onClientCommonPlayerAttributesChange(event);
        });
    eventbus.subscribe<PlayerDashEvent>([this](const PlayerDashEvent &event) {
        this->onPlayerDash(event);
    });
    eventbus.subscribe<SkillReadyEvent>([this](const SkillReadyEvent &event) {
        this->onSkillReady(event);
    });
    eventbus.subscribe<SkillApplyEvent>([this](const SkillApplyEvent &event) {
        this->onSkillApplied(event);
    });
    eventbus.subscribe<SkillEndEvent>([this](const SkillEndEvent &event) {
        this->onSkillEnd(event);
    });
    eventbus.subscribe<EntityDeathEvent>([this](const EntityDeathEvent &event) {
       this->onEntityDeath(event);
    });
    eventbus.subscribe<PlayerRespawnEvent>([this](const PlayerRespawnEvent &event) {
        this->onPlayerRespawn(event);
    });
    eventbus.subscribe<PlayerGlowSetEvent>([this](const PlayerGlowSetEvent &event) {
        this->onPlayerGlowSetEvent(event);
    });
    {
        aoiSignature_.set(Coordinator::getComponentTypeID<EntityType>(), true);
        aoiSignature_.set(Coordinator::getComponentTypeID<Transform>(), true);
        aoiSignature_.set(Coordinator::getComponentTypeID<Size>(), true);
        aoiSignature_.set(Coordinator::getComponentTypeID<NetSyncComp>(), true);
        coord_.registerSystem(aoiSignature_);
    }
}
void NetworkSyncSystem::update(float dt) {
    auto& grid = coord_.ctx<GridResource>();
    auto& peerEntities = coord_.getEntitiesWith(signature_);
    for (Entity peerEntity : peerEntities) {
        ENetPeer* peer = coord_.getComponent<NetworkPeer>(peerEntity).peer;
        PeerAOI& aoi = peerAOIs[peer];
        aoi.last = aoi.current;
        aoi.current.reset();
        aoi.enterList.clear();
        aoi.leaveList.clear();
        aoi.dynamicList.clear();
        aoi.deathList.clear();
        const auto& transform = coord_.getComponent<Transform>(peerEntity);
        int row = static_cast<int>(transform.y / grid.cellHeight_);
        int col = static_cast<int>(transform.x / grid.cellWidth_);
        if (col - GameData::ENTITY_SYNC_RADIUS_X < -1) col = -1 + GameData::ENTITY_SYNC_RADIUS_X;
        else if (col + GameData::ENTITY_SYNC_RADIUS_X > grid.cols_) col = grid.cols_ - GameData::ENTITY_SYNC_RADIUS_X;
        if (row - GameData::ENTITY_SYNC_RADIUS_Y < -1) row = -1 + GameData::ENTITY_SYNC_RADIUS_Y;
        else if (row + GameData::ENTITY_SYNC_RADIUS_Y > grid.rows_) row = grid.rows_ - GameData::ENTITY_SYNC_RADIUS_Y;  //if player on border, adjust area
        for (int dc = -GameData::ENTITY_SYNC_RADIUS_X; dc <= GameData::ENTITY_SYNC_RADIUS_X; ++dc) {   //update visible entities
            for (int dr = -GameData::ENTITY_SYNC_RADIUS_Y; dr <= GameData::ENTITY_SYNC_RADIUS_Y; ++dr) {
                int r = row + dr;
                int c = col + dc;
                if (grid.cellExistAt(r, c)) {
                    const auto& cell = grid.cellAt(r, c);
                    for (Entity e : cell.entities) {
                        if (coord_.hasSignature(e, aoiSignature_) && e != peerEntity) { // && e != peerEntity
                            aoi.current.set(e);
                        }
                    }
                }
            }
        }
        aoi.enterBits = aoi.current & ~aoi.last;
        aoi.leaveBits= aoi.last & ~aoi.current & ~deathEntities_;  //do not send leave for dead entities
        aoi.deathBits = aoi.current & deathEntities_;
        extractIDs(aoi.enterBits, aoi.enterList);
        extractIDs(aoi.leaveBits, aoi.leaveList);
        extractIDs(aoi.current, aoi.dynamicList);
        extractIDs(aoi.deathBits, aoi.deathList);
        PacketWriter& writer = server_.getPacketWriter();
        {  //send static data for entities first enter AOI
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.enterList) {
                if (!writer.canWrite(25)) {
                    driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_STATIC_DATA, true);
                    writer.clearBuffer();
                }
                const auto& entityType = coord_.getComponent<EntityType>(e);
                const auto& entityTransform = coord_.getComponent<Transform>(e);
                const auto& size = coord_.getComponent<Size>(e);
                writer.writeInt16(e)
                    .writeInt8(static_cast<std::uint8_t>(entityType.entityID))
                    .writeInt8(ltonSize8(size.size))
                    .writeInt16(level_.ltonX(entityTransform.x))
                    .writeInt16(level_.ltonY(entityTransform.y));
                if (coord_.hasComponent<NameTag>(e)) {
                    writer.writeInt8(static_cast<std::uint8_t>(1));  //has id
                    writer.writeStr(coord_.getComponent<NameTag>(e).name.data(), 16);
                }
                else {
                    writer.writeInt8(static_cast<std::uint8_t>(0));  //has no id
                }
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_STATIC_DATA, true);
                writer.clearBuffer();
            }
        }
        {  //send death entity IDs
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.deathList) {
                if (!writer.canWrite(2)) {
                    driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_DEATH, true);
                    writer.clearBuffer();
                }
                writer.writeInt16(e);
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_DEATH, true);
                writer.clearBuffer();
            }
        }
        {  //send leave entity IDs
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.leaveList) {
                if (!writer.canWrite(2)) {
                    driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_LEAVE, true);
                    writer.clearBuffer();
                }
                writer.writeInt16(e);
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_LEAVE, true);
                writer.clearBuffer();
            }
        }
        {  //send dynamic data for all visible entities
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.dynamicList) {
                if ((level_.getCurrentTick() + coord_.getComponent<NetSyncComp>(e).offset) %
                    TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC) {
                    continue;   //not this tick
                }
                if (!writer.canWrite(6)) {
                    driver.send(writer.takePacket(), peer, 1, ClientTypes::PKT_ENTITY_DYNAMIC_DATA, false);
                    writer.clearBuffer();
                }
                const auto& entityTransform = coord_.getComponent<Transform>(e);
                writer.writeInt16(e).writeInt16(level_.ltonX(entityTransform.x)).writeInt16(level_.ltonY(entityTransform.y));
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 1, ClientTypes::PKT_ENTITY_DYNAMIC_DATA, false);
                writer.clearBuffer();
            }
        }
        {  //handle entity size packet
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.dynamicList) {
                if (sizeChangedBits_.test(e)) {
                    if (!writer.canWrite(3)) {
                        driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_SIZE_CHANGE, true);
                        writer.clearBuffer();
                    }
                    const auto& size = coord_.getComponent<Size>(e);
                    writer.writeInt16(e).writeInt8(ltonSize8(size.size));
                }
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_SIZE_CHANGE, true);
                writer.clearBuffer();
            }
            sizeChangedBits_.reset();
        }
        {  //handle entity hp packet
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.dynamicList) {
                if (hpChangedBits_.test(e)) {
                    //dont send if it is player
                    if (coord_.hasComponent<NetworkPeer>(e) && coord_.getComponent<NetworkPeer>(e).peer == peer) {
                        continue;
                    }
                    if (!writer.canWrite(3)) {
                        driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_HP_CHANGE, true);
                        writer.clearBuffer();
                    }
                    const auto&[hp, maxHp, inc] = coord_.getComponent<HP>(e);
                    writer.writeInt16(e).writeInt8(static_cast<std::uint8_t>(hp / maxHp * 255.f));
                }
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_HP_CHANGE, true);
                writer.clearBuffer();
            }
        }
        {  //handle entity hp delta packet
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.dynamicList) {
                if (hpChangedBits_.test(e)) {
                    //don't send if it is player, will send in player state packet
                    if (coord_.hasComponent<NetworkPeer>(e) && coord_.getComponent<NetworkPeer>(e).peer == peer) {
                        continue;
                    }
                    const auto& it = hpDeltaList_.find(e);
                    if (it != hpDeltaList_.end()) {
                        for (float delta : it->second) {
                            if (!writer.canWrite(4)) {
                                driver.send(writer.takePacket(), peer, 1, ClientTypes::PKT_ENTITY_HP_DELTA, true);
                                writer.clearBuffer();
                            }
                            writer.writeInt16(e).writeInt16(ltonHPDelta(delta));
                        }
                    }
                }
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 1, ClientTypes::PKT_ENTITY_HP_DELTA, true);
                writer.clearBuffer();
            }
        }
        //here for player entity sync. players should be treated differently because they need updates with higher accuracy
        {  //send player hp and fp packet (playerState)
            const auto &hpComp = coord_.getComponent<HP>(peerEntity);
            const auto &fpComp = coord_.getComponent<FP>(peerEntity);
            const auto &sizeComp = coord_.getComponent<Size>(peerEntity);
            std::uint16_t netHP = ltonHP16(hpComp.hp);
            std::uint16_t netFP = ltonFP(fpComp.fp);
            std::uint16_t netSize = ltonSize16(sizeComp.size);
            std::uint16_t netTime = ltonTime(coord_.ctx<TimeResource>().currentTime);
            if (//(hpComp.hp != aoi.lastHP || fpComp.fp != aoi.lastFP) &&
                !((level_.getCurrentTick() + peerEntity) % TICKS_PLAYER_STATE_UPDATE)) {
                ServerNetworkDriver &driver = server_.getNetworkDriver();
                writer.writeInt16(netHP).writeInt16(netFP).writeInt16(netSize).writeInt16(netTime);
                const auto& it = hpDeltaList_.find(peerEntity);
                if (it != hpDeltaList_.end()) {
                    constexpr int MAX_PLAYER_HP_DELTA_PER_PACKET = 10;
                    int curDelta = 0;
                    for (float delta : it->second) {
                        if (curDelta >= MAX_PLAYER_HP_DELTA_PER_PACKET) break;
                        ++curDelta;
                        writer.writeInt16(ltonHPDelta(delta));
                    }
                }
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_PLAYER_STATE_UPDATE, false);
                writer.clearBuffer();
                aoi.lastHP = hpComp.hp;
                aoi.lastFP = fpComp.fp;
            }
        }
        aoi.current = aoi.current & ~deathEntities_;  //do not include dead entities in current AOI
        server_.getNetworkDriver().flush();  //important TAT
    }
    for (auto& pair : hpDeltaList_) {  //clear, avoid memory consumption
        pair.second.clear();
    }
    deathEntities_.reset();
    hpChangedBits_.reset();
}
