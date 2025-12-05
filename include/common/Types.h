//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_TYPES_H
#define UNDEROCEAN_TYPES_H
#include <bitset>
#include "net(depricate)/enet.h"
#include "net(depricate)/PacketChannel.h"
//x-macro
#define ENTITY_TYPES \
X(SMALL_YELLOW) \
X(ROUND_GREEN) \
X(BALL_ORANGE) \
X(FLY_FISH) \
X(BLUE_LONG) \
X(RED_LIGHT) \
X(UGLY_FISH) \
X(SMALL_SHARK)

using Entity = std::uint16_t;
constexpr Entity MAX_ENTITIES = 4096;     //512 byte
using ComponentType = std::uint8_t;
constexpr ComponentType MAX_COMPONENTS = 32;
using ResourceType = std::size_t;
using Signature = std::bitset<MAX_COMPONENTS>;
constexpr float ENTITY_MAX_SIZE = 200.f;
enum class EntityTypeID : std::uint8_t {
    NONE = 0,
#define X(name) name,
    ENTITY_TYPES
#undef X
    COUNT
};
namespace ServerTypes {  //packet that server handle
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
namespace ClientTypes {  //packet that client handle
    enum PacketType : std::uint8_t {
        PKT_ENTITY_STATIC_DATA = 0,  //for entity first enter aoi, reliable
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
    EntityTypeID type = EntityTypeID::NONE;
    std::uint8_t netSize = 0;  //actually ENTITY_MAX_SIZE / netSize
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
template<EntityTypeID ID>
struct ParamTable;
template<> struct ParamTable<EntityTypeID::SMALL_YELLOW> {
    static constexpr float MAX_VELOCITY = 10.f;
    static constexpr int PERCEPTION_DIST = 1;   //radius by chunk fish can see
    static constexpr float NEIGHBOR_RADIUS2 = 100.f;    //boids
    static constexpr float SEPARATION_RADIUS2 = 50.f;
    static constexpr float AVOID_RADIUS2 = 300.f;
    static constexpr float COHESION_WEIGHT = 20.f;
    static constexpr float SEPARATION_WEIGHT = 1000.f;
    static constexpr float ALIGNMENT_WEIGHT = 100.f;
    static constexpr float AVOID_WEIGHT = 2.f;
};
#endif //UNDEROCEAN_TYPES_H