//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_ENETCLIENTCHANNEL_H
#define UNDEROCEAN_ENETCLIENTCHANNEL_H
#include "common/Types.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "ClientPacketChannel.h"
#include "enet.h"

class ENetClientChannel : public ClientPacketChannel<ENetClientChannel> {
public:
    ENetClientChannel() : clientHost_(nullptr), serverPeer_(nullptr) {}
    ~ENetClientChannel() {
        if (serverPeer_) {
            enet_peer_disconnect(serverPeer_, 0);
        }
        if (clientHost_) {
            enet_host_destroy(clientHost_);
        }
    }
    bool connect(const std::string& ip, int port) {
        clientHost_ = enet_host_create(nullptr, 1, 2, 0, 0);
        if (clientHost_ == nullptr) {
            return false;
        }
        ENetAddress address;
        enet_address_set_host(&address, ip.c_str());
        address.port = static_cast<enet_uint16>(port);
        serverPeer_ = enet_host_connect(clientHost_, &address, 2, 0);
        if (serverPeer_ == nullptr) {
            return false;
        }
        return true;
    }
    void sendUnreliablePacketImpl(Packet* packet) {
        if (!serverPeer_) return;
        ENetPacket* enetPacket = enet_packet_create(packet->data(), packet->size(), ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
        enet_peer_send(serverPeer_, 1, enetPacket);
    }

    void sendReliablePacketImpl(Packet* packet) {
        if (!serverPeer_) return;
        ENetPacket* enetPacket = enet_packet_create(packet->data(), packet->size(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(serverPeer_, 0, enetPacket);
    }
    std::unique_ptr<Packet> popPacketImpl() {
        if (!clientHost_) return nullptr;
        ENetEvent event;
        while (enet_host_service(clientHost_, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    auto packet = std::make_unique<Packet>(  //deep copy
                        event.packet->data,
                        event.packet->data + event.packet->dataLength
                    );
                    enet_packet_destroy(event.packet);
                    return packet;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    event.peer->data = nullptr;
                    serverPeer_ = nullptr;
                    {
                        auto disconnectPacket = std::make_unique<Packet>();
                        disconnectPacket->push_back(PKT_DISCONNECT);
                        return disconnectPacket;
                    }
                case ENET_EVENT_TYPE_CONNECT:
                    {
                        auto connectPacket = std::make_unique<Packet>();
                        connectPacket->push_back(PKT_CONNECT);
                        return connectPacket;
                    }
                default:
                    break;
            }
        }
        return nullptr;
    }
private:
    ENetHost* clientHost_;
    ENetPeer* serverPeer_;
};

#endif //UNDEROCEAN_ENETCLIENTCHANNEL_H