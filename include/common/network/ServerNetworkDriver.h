//
// Created by 15201 on 11/24/2025.
//

#ifndef UNDEROCEAN_NETWORKDRIVER_H
#define UNDEROCEAN_NETWORKDRIVER_H
#include <iostream>
#include <queue>
#include "common/Types.h"
#include "common/net(depricate)/enet.h"
using namespace ServerTypes;
//two channels, 1 for message, 0 for others
class ServerNetworkDriver {   //receive packet, send packet and distribute by channels, responsible for handle packet type
public:
    ServerNetworkDriver() : serverHost_(nullptr) {}
    ~ServerNetworkDriver() {
        if (serverHost_) {
            enet_host_destroy(serverHost_);
        }
    }
    bool listen(int port) {
        ENetAddress address;
        enet_address_set_host(&address, "127.0.0.1");
        //address.host = ENET_HOST_ANY;
        address.port = static_cast<enet_uint16>(port);
        serverHost_ = enet_host_create(&address, SERVER_MAX_CONNECTIONS, 2, 0, 0);
        if (serverHost_ == nullptr) {
            DWORD err = WSAGetLastError();
            std::cerr << "enet_host_create failed. WSA Error: " << err << " " << address.port << std::endl;
            return false;
        }
        // enet_host_set_checksum_callback(serverHost_, enet_crc32); if crc32 needed
        return true;
    }
    void send(const Packet* packet, ENetPeer* peer, int channel, int packetType, bool reliable = false) {  //NOTE: packetType must be valid
        if (!peer || !packet) return;
        size_t total = packet->size() + 1; //extra byte for PacketTypeID
        ENetPacket* enetPacket = enet_packet_create(nullptr, total,
            !reliable ? ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT : ENET_PACKET_FLAG_RELIABLE);
        if (!enetPacket) return;
        enetPacket->data[0] = static_cast<uint8_t>(packetType);
        std::memcpy(enetPacket->data + 1, packet->data(), packet->size());
        enet_peer_send(peer, channel, enetPacket);
    }
    void pollPackets();
    std::unique_ptr<NamedPacket> popPacket(int packetType) {  //caution: must update peer list
        if (packets_[packetType].empty()) {
            return nullptr;
        }
        auto namedPacket = std::move(packets_[packetType].front());
        packets_[packetType].pop();
        return namedPacket;
    }
    [[nodiscard]] bool hasPacket(int packetType) const {
        if (packets_[packetType].empty()) return false;
        return true;
    }
    [[nodiscard]] ENetHost* getHost() const { return serverHost_; }
    ServerNetworkDriver(ServerNetworkDriver const&) = delete;
private:
    ENetHost* serverHost_;
    std::array<std::queue<std::unique_ptr<NamedPacket>>, PacketType::COUNT> packets_;  //diff types
};

inline void ServerNetworkDriver::pollPackets() {
    if (!serverHost_) return;
    ENetEvent event;
    while (enet_host_service(serverHost_, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                auto namedPacket = std::make_unique<NamedPacket>();
                namedPacket->peer = event.peer;
                packets_[PKT_CONNECT].push(std::move(namedPacket)); //connect packet
                enet_peer_timeout(event.peer, PING_TIMES, PING_TIMEOUT_MIN, PING_TIMEOUT_MAX);
                enet_peer_ping_interval(event.peer, HEARTBEAT_INTERVAL);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                if (event.packet->dataLength < 1) {  //empty packet
                    enet_packet_destroy(event.packet);
                    break;
                }
                uint8_t typeByte = event.packet->data[0];
                if (typeByte >= PacketType::COUNT) break; //type bad
                auto namedPacket = std::make_unique<NamedPacket>();
                namedPacket->peer = event.peer;
                namedPacket->packet.assign(
                    event.packet->data + 1,   //remove type!
                    event.packet->data + event.packet->dataLength
                );
                packets_[typeByte].push(std::move(namedPacket));
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            case ENET_EVENT_TYPE_DISCONNECT: {
                auto namedPacket = std::make_unique<NamedPacket>();
                namedPacket->peer = event.peer;
                packets_[PKT_DISCONNECT].push(std::move(namedPacket)); //disconnect packet
                break;
            }
            default:
                break;
        }
    }
}
#endif //UNDEROCEAN_NETWORKDRIVER_H
