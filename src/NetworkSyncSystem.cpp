
#include "server/new/system/NetworkSyncSystem.h"

#include "common/network/ServerNetworkDriver.h"
#include "server/GameServer.h"

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
                        if (e != peerEntity) {
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
        {  //send static data for entities first enter AOI
            for (Entity e : aoi.enterList) {
                ServerNetworkDriver driver = server_.getNetworkDriver();
                Packet packet;

            }
        }
    }
}
