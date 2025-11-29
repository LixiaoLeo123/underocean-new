//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_TYPES_H
#define UNDEROCEAN_TYPES_H
#include <cstdint>
#include <bitset>
#include "net(depricate)/enet.h"
#include "net(depricate)/PacketChannel.h"

using Entity = std::uint16_t;
const Entity MAX_ENTITIES = 5000;
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;
using ResourceType = std::size_t;
using Signature = std::bitset<MAX_COMPONENTS>;
namespace ServerTypes {
    enum PacketType : std::uint8_t {
        PKT_CONNECT = 0,   //0 byte
        PKT_DISCONNECT = 1,  //same
        PKT_LEVEL_CHANGE = 2,  //1 byte for to
        PKT_MESSAGE = 3,   //size unknown
        PKT_TRANSFORM = 4,  //for server, 2*2 byte
        PKT_ACTION = 5,
        PKT_LOGIN = 6,
        COUNT
    };
}
namespace ClientTypes {
    enum PacketType : std::uint8_t {
        COUNT
    };
}
// constexpr int getPayloadBytes(PacketType type) {
//     switch (type) {
//         case PKT_CONNECT:     return 0;
//         case PKT_DISCONNECT:  return 0;
//         case PKT_LEVEL_CHANGE: return 2; //from, to
//         case PKT_MESSAGE:     return 16;  //can be connected if over 16
//         default:              return 0;
//     }
// }
constexpr int SERVER_MAX_CONNECTIONS = 32;
constexpr int SERVER_MAX_BUFFER_SIZE = 100; //100 packets in buffer max!
constexpr int HEARTBEAT_INTERVAL = 5000;  //by milliseconds, depricate
constexpr int PING_TIMES = 32;   //times that tried to ack
constexpr int PING_TIMEOUT_MIN = 5;
constexpr int PING_TIMEOUT_MAX = 10;
constexpr int CELL_INIT_RESERVATION = 32;   //see GridResource
struct PlayerData {  //related to GameServer::handleLoginPacket()!!
    ENetPeer* peer = nullptr;
    bool hasLogin = false;
    int currentLevel = 0;
    char playerId[16] = "";
    std::uint16_t netX = 0;  //level divided into grid, 0 - 65535
    std::uint16_t netY = 0;
};
// struct LevelChangeRequest {
//     ENetPeer* peer;
//     std::uint8_t fromLevel;
//     std::uint8_t toLevel;
//     LevelChangeRequest(ENetPeer* p, std::uint8_t fromL, std::uint8_t toL)
//         : peer(p), fromLevel(fromL), toLevel(toL) {}
// };
using Packet = std::vector<uint8_t>;
struct NamedPacket {
    ENetPeer* peer;
    Packet packet;
};
enum class MsgId : std::uint8_t {    //local message
    COUNT
};
inline std::string getLocalString(MsgId id) {  //same
    switch (id) {
        default:                    return "";
    }
}
#endif //UNDEROCEAN_TYPES_H