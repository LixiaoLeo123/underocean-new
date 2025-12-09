//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_TYPES_H
#define UNDEROCEAN_TYPES_H
#include <bitset>
#include "net(depricate)/enet.h"
#include "net(depricate)/PacketChannel.h"
#include <cmath>
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
constexpr Entity MAX_ENTITIES = 16384;     //2048 byte
using ComponentType = std::uint8_t;
constexpr ComponentType MAX_COMPONENTS = 32;
using ResourceType = std::size_t;
using Signature = std::bitset<MAX_COMPONENTS>;
constexpr float ENTITY_MAX_SIZE = 200.f;
inline std::uint8_t ltonSize(float size) {    //local size to net size
    return static_cast<std::uint8_t>(std::round(size / ENTITY_MAX_SIZE * 255.f));
}
inline float ntolSize(std::uint8_t netSize) {    //net size to local size
    return static_cast<float>(netSize) / 255.f * ENTITY_MAX_SIZE;
}
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
        PKT_MESSAGE = 3,   //size unknown, both server and client use channel 1
        PKT_TRANSFORM = 4,  //for server, 2*2 byte
        PKT_ACTION = 5,
        PKT_LOGIN = 6,
        // char[16] playerId; uint8 type; uint8 size, total 18 byte
        COUNT
    };
}
namespace ClientTypes {  //packet that client handle
    enum PacketType : std::uint8_t {
        PKT_ENTITY_STATIC_DATA = 0,  //for entity first enter aoi, reliable
        //2 entityID 1 type 1 size 2 netX 2 netY = 8 byte * n
        PKT_ENTITY_DYNAMIC_DATA = 1, //for entity transform update, unreliable
        //2 entityID 2 netX 2 netY * n
        PKT_ENTITY_LEAVE = 2,   //for entity leave aoi, reliable
        //2 entityID * n
        PKT_ENTITY_SIZE_CHANGE = 3, //reliable
        //2 entityID 1 newSize
        PKT_MESSAGE = 4,
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
constexpr int HEARTBEAT_INTERVAL = 5000;  //by milliseconds
constexpr int PING_TIMES = 128;   //times that tried to ack
constexpr int PING_TIMEOUT_MIN = 10000;
constexpr int PING_TIMEOUT_MAX = 20000;
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
    static constexpr float MAX_ACCELERATION = 40.f;
    static constexpr float INIT_SIZE = 6.f;
    static constexpr int PERCEPTION_DIST = 3;   //radius by chunk fish can see
    static constexpr float NEIGHBOR_RADIUS2 = 100.f;    //boids
    static constexpr float SEPARATION_RADIUS2 = 50.f;
    static constexpr float AVOID_RADIUS2 = 300.f;
    static constexpr float COHESION_WEIGHT = 200.f;
    static constexpr float SEPARATION_WEIGHT = 1000.f;
    static constexpr float ALIGNMENT_WEIGHT = 100.f;
    static constexpr float AVOID_WEIGHT = 2.f;
};
struct EntitySizeChangeEvent {
    Entity entity;
    float newSize;
};
constexpr const char* getTexturePath(EntityTypeID type) {
    using ET = EntityTypeID;
    constexpr const char* paths[] = {
        "images/fish/none.png",           // NONE = 0
        "images/fish/small_yellow.png",   // SMALL_YELLOW
        "images/fish/round_green.png",    // ROUND_GREEN
        "images/fish/ball_orange.png",    // BALL_ORANGE
        "images/fish/fly_fish.png",       // FLY_FISH
        "images/fish/blue_long.png",      // BLUE_LONG
        "images/fish/red_light.png",      // RED_LIGHT
        "images/fish/ugly_fish.png",      // UGLY_FISH
        "images/fish/small_shark.png"     // SMALL_SHARK
    };
    static_assert(sizeof(paths) / sizeof(paths[0]) == static_cast<size_t>(ET::COUNT),
                  "Texture path array size mismatch!");
    auto idx = static_cast<std::size_t>(type);
    return paths[idx];
}
constexpr int getTextureTotalFrame(EntityTypeID type) {
    constexpr int frames[] = {
        1,  // NONE = 0
        2,  // SMALL_YELLOW
        2,  // ROUND_GREEN
        2,  // BALL_ORANGE
        2,  // FLY_FISH
        2,  // BLUE_LONG
        2,  // RED_LIGHT
        2,  // UGLY_FISH
        2   // SMALL_SHARK
    };
    static_assert(sizeof(frames) / sizeof(frames[0]) == static_cast<size_t>(static_cast<EntityTypeID>(EntityTypeID::COUNT)),
                  "Texture frame array size mismatch!");
    auto idx = static_cast<std::size_t>(type);
    return frames[idx];
}
constexpr float getFrameInterval(EntityTypeID type) {
    switch (type) {
        case EntityTypeID::SMALL_YELLOW:
        case EntityTypeID::ROUND_GREEN:
        case EntityTypeID::BALL_ORANGE:
        case EntityTypeID::FLY_FISH:
        case EntityTypeID::BLUE_LONG:
        case EntityTypeID::RED_LIGHT:
        case EntityTypeID::UGLY_FISH:
        case EntityTypeID::SMALL_SHARK:
            return 1.f;   //5 fps
        default:
            return 1.f;   //1 fps
    }
}
#endif //UNDEROCEAN_TYPES_H