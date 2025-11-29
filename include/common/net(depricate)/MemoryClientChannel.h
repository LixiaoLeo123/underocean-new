//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_MEMORYCLIENTCHANNEL_H
#define UNDEROCEAN_MEMORYCLIENTCHANNEL_H
#include "ClientPacketChannel.h"
#include "MemoryServerChannel.h"

class MemoryClientChannel : public ClientPacketChannel<MemoryClientChannel>{
public:
    void setServerChannel(MemoryServerChannel* serverChannel){ serverChannel_ = serverChannel; }
    void sendUnreliablePacketImpl(Packet* packet){
        serverChannel_->packets_.push(std::make_unique<Packet>(*packet));
    }
    void sendReliablePacketImpl(Packet* packet){
        sendUnreliablePacketImpl(packet);
    }
    std::unique_ptr<Packet> popPacketImpl() {
        std::unique_ptr<Packet> packet = std::move(packets_.front());
        packets_.pop();
        return std::move(packet);
    }
private:
    MemoryServerChannel* serverChannel_ = nullptr;
    std::queue<std::unique_ptr<Packet>> packets_;
    friend class MemoryServerChannel;
};
#endif //UNDEROCEAN_MEMORYCLIENTCHANNEL_H