//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_STATEMANAGER_H
#define UNDEROCEAN_STATEMANAGER_H
#include <array>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>

#include "common/network/ServerNetworkDriver.h"
#include "common/Types.h"
#include "common/net(depricate)/PacketReader.h"
#include "common/net(depricate)/PacketWriter.h"
#include "core/GameData.h"
#include "new/ILevel.h"
class PacketWriter;
using namespace ServerTypes;
class GameServer {
private:
    static constexpr int levelNum = 2;   //max level num is 6
    std::array<std::unique_ptr<ILevel>, levelNum> levels_;  //level0 for lobby!
    static bool isLevelLegal(int level){ return (level >= 0 && level < levelNum); }
    ServerNetworkDriver networkDriver_;
    PacketWriter writer;   //unified packet writer to reduce allocation
    void handleConnectionPacket();
    void tryRemovePlayer(ENetPeer* peer);
    void handleLoginPacket();
    void handleLevelChangePacket();
    void handleMessagePacket();  //only distribute, broadcast. in channel 1
    void handleTransformPacket();   //net pos, need to be converted by level
    void handleActionPacket();
    void broadcast(std::string message);   //better under 1024 bytes
public:
    explicit GameServer();
    GameServer(const GameServer&) = delete;
    GameServer& operator=(const GameServer&) = delete;
    std::unordered_map<ENetPeer*, PlayerData> playerList_;
    std::unordered_map<ENetPeer*, std::queue<std::unique_ptr<NamedPacket>>> buffer_;  //maybe only for actions
    [[nodiscard]] PacketWriter& getPacketWriter() { return writer; }
    void update(float dt) {
        networkDriver_.pollPackets();
        handleConnectionPacket();   //CAUTION: may cause peer existence change!
        handleLoginPacket();
        handleLevelChangePacket();
        handleMessagePacket();
        handleTransformPacket();
        handleActionPacket();   //add to buffer_
        for (auto& level : levels_) {
            level->update(dt);
        }
    }
    [[nodiscard]] ServerNetworkDriver& getNetworkDriver() { return networkDriver_; }
    [[noreturn]] void run() {
        const float dt = 1.0f / static_cast<float>(GameData::SERVER_TPS);
        auto lastTime = std::chrono::steady_clock::now();
        for (;;) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count() / 1e6f;
            if (elapsed >= dt) {
                update(elapsed);
                lastTime = now;
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>((dt - elapsed) * 1e6)));
            }
        }
    }
};
inline void GameServer::handleConnectionPacket() {  //connect or disconnect
    while (networkDriver_.hasPacket(PKT_CONNECT)) {  //handle messages
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_CONNECT));
        ENetPeer* peer = namedPacket->peer;
        playerList_[peer] = PlayerData{};
        buffer_.try_emplace(peer);
        levels_[0]->onPlayerJoin(peer);
    }
    while (networkDriver_.hasPacket(PKT_DISCONNECT)) {  //handle messages
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_DISCONNECT));
        ENetPeer* peer = namedPacket->peer;
        tryRemovePlayer(peer);
        break;
    }
}
inline void GameServer::tryRemovePlayer(ENetPeer* peer) {
    auto it = playerList_.find(peer);
    if (it != playerList_.end()) {
        levels_[it->second.currentLevel]->onPlayerLeave(peer);
        broadcast("&e" + std::string(it->second.playerId).append(" left the game"));
        playerList_.erase(it);
    }
    buffer_.erase(peer);
}
inline void GameServer::handleLevelChangePacket() {
    while (networkDriver_.hasPacket(PKT_LEVEL_CHANGE)) {
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_LEVEL_CHANGE));
        ENetPeer* peer = namedPacket->peer;
        auto it = playerList_.find(peer);
        if (it == playerList_.end()) continue;  //already leave
        if (!it->second.hasLogin) continue;
        Packet& packet = namedPacket->packet;
        if (packet.size() != 1) continue;
        int to = packet[0];
        if (!isLevelLegal(to)) continue;  //bad packet
        levels_[it->second.currentLevel]->onPlayerLeave(peer);
        it->second.currentLevel = to;
        levels_[to]->onPlayerJoin(peer);
        buffer_[peer] = std::queue<std::unique_ptr<NamedPacket>>{}; //clear buffer
    }
}
inline void GameServer::handleMessagePacket() {  //only distribute, broadcast
    while (networkDriver_.hasPacket(PKT_MESSAGE)) {
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_MESSAGE));
        for (auto pair : playerList_) {
            networkDriver_.send(&namedPacket->packet, pair.first, 1, ClientTypes::PKT_MESSAGE, true);
        }
    }
}
inline void GameServer::broadcast(std::string message) {
    if (message.size() > 10000) return;   //large packet, dont handle
    std::vector<std::uint8_t> packet;
    packet.insert(packet.end(), message.begin(), message.end());
    for (auto pair : playerList_) {
        networkDriver_.send(&packet, pair.first, 1, ClientTypes::PKT_MESSAGE, true);
    }
}

inline void GameServer::handleTransformPacket() {
    while (networkDriver_.hasPacket(PKT_TRANSFORM)) {
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_TRANSFORM));
        ENetPeer* peer = namedPacket->peer;
        auto it = playerList_.find(peer);
        if (it == playerList_.end()) continue;   //dont exist, dont handle
        Packet& packet = namedPacket->packet;
        if (packet.size() != 4) continue;   //size wrong, dont handle
        PacketReader reader(std::move(packet));
        it->second.netX = reader.nextUInt16();  //update net pos
        it->second.netY = reader.nextUInt16();
    }
}

inline void GameServer::handleActionPacket() {
    while (networkDriver_.hasPacket(PKT_ACTION)) {
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_ACTION));
        ENetPeer* peer = namedPacket->peer;
        auto it = playerList_.find(peer);
        if (it == playerList_.end()) continue;  //dont exist
        if (buffer_[peer].size() >= SERVER_MAX_BUFFER_SIZE) buffer_[peer].pop();
        buffer_[peer].push(std::move(namedPacket));
    }
}
#endif //UNDEROCEAN_STATEMANAGER_H
