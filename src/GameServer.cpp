#include "server/GameServer.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

#include "server/core(deprecate)/GameData.h"
#include "server/new/levels/Level0.h"
#include "server/new/levels/Level1.h"

GameServer::GameServer(){
    if (!networkDriver_.listen(GameData::SERVER_PORT)) {
        throw std::runtime_error("Error starting server");
    }
    levels_[0] = std::make_unique<Level0>();
    levels_[1] = std::make_unique<Level1>(*this);
    std::cout << "Server started" << std::endl;
}
void GameServer::handleLoginPacket() {   //char[16] name; uint8 type;
    while (networkDriver_.hasPacket(PKT_LOGIN)) {
        std::unique_ptr<NamedPacket> namedPacket = std::move(networkDriver_.popPacket(PKT_LOGIN));
        ENetPeer* peer = namedPacket->peer;
        auto it = playerList_.find(peer);
        if (it == playerList_.end()) continue;  //already leave
        Packet& packet = namedPacket->packet;
        if (packet.size() != 18) continue;    //size change here
        //deserialize start
        std::copy_n(packet.begin(), 16, it->second.playerId);  //playerId
        {  //type
            std::uint8_t temp;
            std::copy_n(packet.begin() + 16, 1, &temp);
            if (temp >= static_cast<std::uint8_t>(EntityTypeID::COUNT)) continue;  //wrong packet
            it->second.type = static_cast<EntityTypeID>(temp);
        }
        std::copy_n(packet.begin() + 17, 1, &it->second.netSize);
        //and...
        it->second.hasLogin = true;   //finish
        broadcast("&e" + std::string(it->second.playerId).append(" joined the game"));
    }
}