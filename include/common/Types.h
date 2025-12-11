//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_TYPES_H
#define UNDEROCEAN_TYPES_H
#include <bitset>
#include "net(depricate)/enet.h"
#include <cmath>
#include <vector>
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
constexpr float ENTITY_MAX_HP = 32768.f;
constexpr float ENTITY_MAX_FP = 32768.f;
inline std::uint8_t ltonSize(float size) {    //local size to net size
    return static_cast<std::uint8_t>(std::round(size / ENTITY_MAX_SIZE * 255.f));
}
inline float ntolSize(std::uint8_t netSize) {    //net size to local size
    return static_cast<float>(netSize) / 255.f * ENTITY_MAX_SIZE;
}
inline std::uint16_t ltonHP16(float hp) {    //local hp to net hp, 16 bits for Player
    return static_cast<std::uint16_t>(std::round(hp / ENTITY_MAX_HP * 65535.f));
}
inline float ntolHP16(std::uint16_t netHP) {    //net hp to local hp
    return static_cast<float>(netHP) / 65535.f * ENTITY_MAX_HP;
}
inline std::uint16_t ltonFP(float fp) {    //local fp to net fp
    return static_cast<std::uint16_t>(std::round(fp / ENTITY_MAX_FP * 65535.f));
}
inline float ntolFP(std::uint16_t netFP) {   //net fp to local fp
    return static_cast<float>(netFP) / 255.f * ENTITY_MAX_FP;
}
inline std::uint16_t ltonHP8(float hp) {    //local hp to net hp
    return static_cast<std::uint16_t>(std::round(hp / ENTITY_MAX_HP * 255.f));
}
inline float ntolHP8(std::uint16_t netHP) {    //net hp to local hp, 8 bits for Entity
    return static_cast<float>(netHP) / 255.f * ENTITY_MAX_HP;
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
        // char[16] playerId; uint8 type; uint8 size, uint16 hp, uint16 fp, total 22 byte
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
        PKT_ENTITY_HP_CHANGE = 4,  //reliable
        //2 entityID 1 newHP
        PKT_MESSAGE = 4,
        // char[]
        PKT_PLAYER_STATE_UPDATE = 5, //reliable, send when hp or fp change
        //1HP 1FP = 2byte
        PKG_FINISH_LOGIN = 6, //reliable, 2 byte for max HP, 2 byte for max FP, total 4 bytes
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
constexpr int SERVER_MAX_BUFFER_SIZE = 100; //100 packets in buffer max! in GameServer
constexpr int NET_MAX_CACHED_PACKETS = 65535; //total in driver
constexpr int HEARTBEAT_INTERVAL = 5000;  //by milliseconds
constexpr int PING_TIMES = 128;   //times that tried to ack
constexpr int PING_TIMEOUT_MIN = 10000;
constexpr int PING_TIMEOUT_MAX = 20000;
constexpr int CELL_INIT_RESERVATION = 32;   //see GridResource
constexpr int TICKS_PER_ENTITY_DYNAMIC_DATA_SYNC = 2;  //every what ticks send dynamic data
constexpr int TICKS_PLAYER_STATE_UPDATE = 3;  //useful when player state change quickly
struct PlayerData {  //related to GameServer::handleLoginPacket(), used by levels, valid check by server, as a temp for level change
    ENetPeer* peer = nullptr;
    bool hasLogin = false;
    int currentLevel = 0;
    char playerId[16] = "";
    std::uint16_t netX = 0;  //level divided into grid, 0 - 65535
    std::uint16_t netY = 0;
    EntityTypeID type = EntityTypeID::NONE;
    float initHP {0.f};  //from 0.f to 1.f
    float initFP {0.f};
    float size {0.f};  //actually ENTITY_MAX_SIZE / netSize
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
    static constexpr float MAX_VELOCITY = 7.f;
    static constexpr float MAX_ACCELERATION = 70.f;
    static constexpr float MASS = 1.f;  //mass proportional to size^2
    static constexpr float INIT_SIZE = 4.2f;  //remember changing GameData init
    static constexpr float SIZE_STEP = 1.4f;  //size increase step
    static constexpr float HP_BASE = 5.f;  //hp proportional to size
    static constexpr float FP_BASE = 10.f;  //fp proportional to size^2
    static constexpr float FP_DEC_RATE_BASE = 0.1f;  //fp decreasing rate per second proportional to size^3
    static constexpr int PERCEPTION_DIST = 3;   //radius by chunk fish can see
    static constexpr float NEIGHBOR_RADIUS2 = 70.f;    //boids
    static constexpr float SEPARATION_RADIUS2 = 50.f;
    static constexpr float AVOID_RADIUS2 = 300.f;
    static constexpr float COHESION_WEIGHT = 500.f;
    static constexpr float SEPARATION_WEIGHT = 550.f;
    static constexpr float ALIGNMENT_WEIGHT = 100.f;
    static constexpr float AVOID_WEIGHT = 2.f;
};
struct EntitySizeChangeEvent {
    Entity entity;
    float newSize;
};
struct EntityHPChangeEvent {
    Entity entity;
    float newHP;
};
struct PlayerLeaveEvent {
    ENetPeer* peer;
};
struct PlayerJoinEvent {
    PlayerData& playerData;
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