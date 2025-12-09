#include "server/new/system/NetworkSyncSystem.h"

#include "common/net(depricate)/PacketWriter.h"
#include "common/network/ServerNetworkDriver.h"
#include "server/GameServer.h"
#include "server/new/LevelBase.h"
#include "server/new/resources/GridResource.h"

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
        const auto& transform = coord_.getComponent<Transform>(peerEntity);
        int row = static_cast<int>(transform.y / grid.cellHeight_);
        int col = static_cast<int>(transform.x / grid.cellWidth_);
        for (int dc = -GameData::ENTITY_SYNC_RADIUS_X; dc <= GameData::ENTITY_SYNC_RADIUS_X; ++dc) {   //update visible entities
            for (int dr = -GameData::ENTITY_SYNC_RADIUS_Y; dr <= GameData::ENTITY_SYNC_RADIUS_Y; ++dr) {
                int r = row + dr;
                int c = col + dc;
                if (grid.cellExistAt(r, c)) {
                    const auto& cell = grid.cellAt(r, c);
                    for (Entity e : cell.entities) {
                        if (e != peerEntity && coord_.hasSignature(e, aoiSignature_)) {
                            aoi.current.set(e);
                        }
                    }
                }
            }
        }
        aoi.enterBits = aoi.current & ~aoi.last;
        aoi.leaveBits= aoi.last & ~aoi.current;
        extractIDs(aoi.enterBits, aoi.enterList);
        extractIDs(aoi.leaveBits, aoi.leaveList);
        extractIDs(aoi.current, aoi.dynamicList);
        PacketWriter& writer = server_.getPacketWriter();
        {  //send static data for entities first enter AOI
            ServerNetworkDriver& driver = server_.getNetworkDriver();
            for (Entity e : aoi.enterList) {
                if (!writer.canWrite(8)) {
                    driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_STATIC_DATA, true);
                    writer.clearBuffer();
                }
                const auto& entityType = coord_.getComponent<EntityType>(e);
                const auto& entityTransform = coord_.getComponent<Transform>(e);
                const auto& size = coord_.getComponent<Size>(e);
                writer.writeInt16(e)
                    .writeInt8(static_cast<std::uint8_t>(entityType.entityID))
                    .writeInt8(ltonSize(size.size))
                    .writeInt16(level_.ltonX(entityTransform.x))
                    .writeInt16(level_.ltonY(entityTransform.y));
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_STATIC_DATA, true);
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
                if (!writer.canWrite(6)) {
                    driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_DYNAMIC_DATA, false);
                    writer.clearBuffer();
                }
                const auto& entityTransform = coord_.getComponent<Transform>(e);
                writer.writeInt16(e).writeInt16(level_.ltonX(entityTransform.x)).writeInt16(level_.ltonY(entityTransform.y));
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_DYNAMIC_DATA, false);
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
                    writer.writeInt16(e).writeInt8(ltonSize(size.size));
                }
            }
            if (!writer.takePacket()->empty()) {
                driver.send(writer.takePacket(), peer, 0, ClientTypes::PKT_ENTITY_SIZE_CHANGE, true);
                writer.clearBuffer();
            }
        }
    }
}
