//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_CLIENTPACKETCHANNEL_H
#define UNDEROCEAN_CLIENTPACKETCHANNEL_H
#include <memory>
#include <vector>

#include "PacketChannel.h"

template <typename Derived>
class ClientPacketChannel{
public:
    void sendReliablePacket(std::unique_ptr<Packet> packet) { //will take ownership of packet
        impl().sendReliablePacketImpl(std::move(packet));
    }
    void sendUnreliablePacket(std::unique_ptr<Packet> packet) { //will take ownership of packet
        impl().sendUnreliablePacketImpl(std::move(packet));
    }
    std::unique_ptr<Packet> popPacket() {
        return std::move(impl().popPacketImpl());
    }
protected:
    Derived& impl() { return static_cast<Derived&>(*this); }
};
#endif //UNDEROCEAN_CLIENTPACKETCHANNEL_H