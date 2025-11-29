//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_ENETSERVERCHANNEL_H
#define UNDEROCEAN_ENETSERVERCHANNEL_H
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>

#include "ServerPacketChannel.h"
#include "common/Types.h"

class ENetServerChannel : public ServerPacketChannel<ENetServerChannel> {
public:
    ENetServerChannel() : serverHost_(nullptr) {}
    ~ENetServerChannel() {
        if (serverHost_) {
            enet_host_destroy(serverHost_);
        }
    }
    bool listen(int port) {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = static_cast<enet_uint16>(port);
        serverHost_ = enet_host_create(&address, SERVER_MAX_CONNECTIONS, 2, 0, 0);
        if (serverHost_ == nullptr) {
            return false;
        }
        // enet_host_set_checksum_callback(serverHost_, enet_crc32); if crc32 needed
        return true;
    }
    void sendUnreliablePacketImpl(Packet* packet, ENetPeer* peer) {
        if (!peer) return;
        ENetPacket* enetPacket = enet_packet_create(packet->data(), packet->size(), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
        enet_peer_send(peer, 1, enetPacket);
    }
    void sendReliablePacketImpl(Packet* packet, ENetPeer* peer) {
        if (!peer) return;
        ENetPacket* enetPacket = enet_packet_create(packet->data(), packet->size(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, enetPacket);
    }
    void broadcastReliablePacketImpl(Packet* packet) {
        for (const auto& pair : packetMailbox_) {
            ENetPacket* enetPacket = enet_packet_create(packet->data(), packet->size(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(pair.first, 0, enetPacket);
        }
    }
    void pollNetworkEventsImpl() {
        if (!serverHost_) return;
        ENetEvent event;
        for (int channel = 0; channel < 2; ++channel) {  //reliable and unreliable
            while (enet_host_service(serverHost_, &event, channel) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT:
                        newlyConnectedPeers_.push(event.peer);
                        packetMailbox_[event.peer] = std::queue<std::unique_ptr<Packet>>();
                        break;
                    case ENET_EVENT_TYPE_RECEIVE: {
                        auto packet = std::make_unique<Packet>(
                            event.packet->data,
                            event.packet->data + event.packet->dataLength
                        );
                        enet_packet_destroy(event.packet);
                        packetMailbox_[event.peer].push(std::move(packet));
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT:
                        newlyDisconnectedPeers_.push(event.peer);
                        packetMailbox_.erase(event.peer);
                        event.peer->data = nullptr;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    ENetPeer* popNewlyConnectedPeerImpl() {
        if (newlyConnectedPeers_.empty()) return nullptr;
        ENetPeer* peer = newlyConnectedPeers_.front();
        newlyConnectedPeers_.pop();
        return peer;
    }
    ENetPeer* popNewlyDisconnectedPeerImpl() {
        if (newlyDisconnectedPeers_.empty()) return nullptr;
        ENetPeer* peer = newlyDisconnectedPeers_.front();
        newlyDisconnectedPeers_.pop();
        return peer;
    }
    std::unique_ptr<Packet> popPacketImpl(ENetPeer* peer) {  //caution: must update peer list
        if (!peer) return nullptr;
        auto it = packetMailbox_.find(peer);
        if (it == packetMailbox_.end()) return nullptr;
        std::queue<std::unique_ptr<Packet>>& mailbox = it->second;
        if (mailbox.empty()) {
            return nullptr;
        }
        auto packet = std::move(mailbox.front());
        mailbox.pop();
        return packet;
    }
    bool hasPacketImpl(ENetPeer* peer) {
        if (!peer) return false;
        auto it = packetMailbox_.find(peer);
        if (it == packetMailbox_.end()) return false;
        return true;
    }
    [[nodiscard]] ENetHost* getHost() const { return serverHost_; }
private:
    ENetHost* serverHost_;
    std::unordered_map<ENetPeer*, std::queue<std::unique_ptr<Packet>>> packetMailbox_;
    std::queue<ENetPeer*> newlyConnectedPeers_;
    std::queue<ENetPeer*> newlyDisconnectedPeers_ ;
};

#endif //UNDEROCEAN_ENETSERVERCHANNEL_H