//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_SERVERPACKETCHANNEL_H
#define UNDEROCEAN_SERVERPACKETCHANNEL_H
#include <memory>
#include "enet.h"
#include "PacketChannel.h"

template <typename Derived>
class ServerPacketChannel{
public:
    void sendReliablePacket(Packet* packet, ENetPeer* peer = nullptr) { //read only
        impl().sendReliablePacketImpl(packet, peer);
    }
    void sendUnreliablePacket(Packet* packet, ENetPeer* peer = nullptr) {
        impl().sendUnreliablePacketImpl(packet, peer);
    }
    void broadcastReliablePacket(Packet* packet) {
        impl().broadcastReliablePacketImpl(packet);
    }
    bool hasPacket(ENetPeer* peer = nullptr) {
        return impl().hasPacketImpl(peer);
    }
    std::unique_ptr<Packet> popPacket(ENetPeer* peer = nullptr) {
        return std::move(impl().popPacketImpl(peer));
    }
    void pollNetworkEvents() {
        return impl().pollNetworkEventsImpl();
    }
    ENetPeer* popNewlyConnectedPeer() {
        return impl().popNewlyConnectedPeerImpl();
    }
    ENetPeer* popNewlyDisconnectedPeer() {
        return impl().popNewlyDisconnectedPeerImpl();
    }
protected:
    Derived& impl() { return static_cast<Derived&>(*this); }
};
#endif //UNDEROCEAN_SERVERPACKETCHANNEL_H