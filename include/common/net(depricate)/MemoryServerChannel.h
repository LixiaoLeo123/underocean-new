//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_MEMORYSERVERCHANNEL_H
#define UNDEROCEAN_MEMORYSERVERCHANNEL_H
#include <queue>

#include "MemoryClientChannel.h"
#include "ServerPacketChannel.h"

class MemoryServerChannel : public ServerPacketChannel<MemoryServerChannel>
{
public:
    void setClientChannel(MemoryClientChannel* clientChannel) { clientChannel_ = clientChannel; }
    void sendUnreliablePacketImpl(Packet* packet, ENetPeer* peer = nullptr) {
        clientChannel_->packets_.push(std::make_unique<Packet>(*packet));  //copy
    }
    void sendReliablePacketImpl(Packet* packet, ENetPeer* peer = nullptr) {
        sendUnreliablePacketImpl(packet);
    }
    void broadcastReliablePacketImpl(Packet* packet) {
        sendReliablePacketImpl(packet);
    }
    std::unique_ptr<Packet> popPacketImpl(ENetPeer* peer = nullptr) {
        std::unique_ptr<Packet> packet = std::move(packets_.front());
        packets_.pop();
        return std::move(packet);
    }
    void pollNetworkEventsImpl() {}
    ENetPeer* popNewlyConnectedPeerImpl() { return nullptr; }
    ENetPeer* popNewlyDisconnectedPeerImpl() { return nullptr; }

private:
    MemoryClientChannel* clientChannel_ = nullptr;
    std::queue<std::unique_ptr<Packet>> packets_;
    friend class MemoryClientChannel;
};
#endif //UNDEROCEAN_MEMORYSERVERCHANNEL_H