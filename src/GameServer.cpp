#include "server/GameServer.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

#include "server/core(deprecate)/GameData.h"
#include "server/new/levels/Level0.h"
#include "server/new/levels/Level1.h"
#include "server/new/system/DerivedAttributeSystem.h"
#include "server/new/system/SkillSystem.h"

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
        if (packet.size() != 23) continue;    //size change here
        //deserialize start
        it->second.peer = peer;
        std::copy_n(packet.begin(), 16, it->second.playerId);  //playerId
        PacketReader reader(std::move(packet));
        reader.jumpBytes(16);
        {  //type
            std::uint8_t temp;
            temp = reader.nextUInt8();
            if (temp >= static_cast<std::uint8_t>(EntityTypeID::COUNT)) continue;  //wrong packet
            it->second.type = static_cast<EntityTypeID>(temp);
        }
        {
            // std::uint8_t tempNetSize;
            // std::uint16_t tempNetInitHP;
            // std::uint16_t tempNetInitFP;
            // std::copy_n(packet.begin() + 17, 1, &tempNetSize);
            // std::copy_n(packet.begin() + 18, 1, &tempNetInitHP);
            // std::copy_n(packet.begin() + 20, 1, &tempNetInitFP);
            it->second.size = ntolSize16(reader.nextUInt16());
            it->second.initHP = ntolHP16(reader.nextUInt16());
            it->second.initFP = ntolFP(reader.nextUInt16());
        }
        //and...
        SkillIndices skillIndices = SkillSystem::getSkillIndices(it->second.type);
        std::uint8_t* skillIndicesArray = skillIndices.skillIndices;
        writer_.writeInt16(ltonHP16(DerivedAttributeSystem::calcMaxHP(it->second.type, it->second.size)))
            .writeInt16(ltonFP(DerivedAttributeSystem::calcMaxFP(it->second.type, it->second.size)))
            .writeInt16(ltonVec(DerivedAttributeSystem::calcMaxVec(it->second.type)))
            .writeInt16(ltonAcc(DerivedAttributeSystem::calcMaxAcc(it->second.type, it->second.size)))
            .writeInt8(skillIndicesArray[0])
            .writeInt8(skillIndicesArray[1])
            .writeInt8(skillIndicesArray[2])
            .writeInt8(skillIndicesArray[3]);
        networkDriver_.send(writer_.takePacket(), peer, 0, ClientTypes::PacketType::PKG_FINISH_LOGIN, true);
        writer_.clearBuffer();
        it->second.hasLogin = true;   //finish
        broadcast("&e" + std::string(it->second.playerId).append(" joined the game"));
    }
}