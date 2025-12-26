//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_COMPONENTS_H
#define UNDEROCEAN_COMPONENTS_H
#include "common/Types.h"
#include "server/new/others/HitBox.h"
#include "server/new/others/UVector.h"
struct UVector;
struct Transform {
    float x = 0, y = 0;
    Transform(UVector vec);
    explicit Transform(float x = 0.f, float y = 0.f) : x(x), y(y) {}
    void operator+=(UVector vec);
    operator UVector() const {
        return UVector{x, y};
    }
};
struct Velocity {
    float vx = 0.f, vy = 0.f;
    Velocity(UVector vec);
    explicit Velocity(float vx = 0.f, float vy = 0.f) : vx(vx), vy(vy) {}
    void operator+=(UVector vec);
    operator UVector() const {
        return UVector{vx, vy};
    }
};
struct Force {
    float ax = 0.f, ay = 0.f;
    Force(UVector vec);
    explicit Force(float ax = 0.f, float ay = 0.f) : ax(ax), ay(ay) {}
    void operator+=(UVector vec);
    operator UVector() const {
        return UVector{ax, ay};
    }
};
struct MaxVelocity {
    float maxVelocity = 10.f;   //current, the constant one see types.h
};
struct MaxAcceleration {
    float maxAcceleration = 10.f;
};
struct EntityType {   //"imagined" bio type, for boids and texture
    EntityTypeID entityID = EntityTypeID::NONE;
};
//the size can decide everything
//Mass proportional to size^2
//HP proportional to size
//FP proportional to size^2
//FP decreasing rate proportional to size^3
//size here float but increase in discrete steps, depending on the type of the fish
struct Size {
    float size = 10.f;  //side length, network uint8_t
};
struct Mass {
    float mass = 0.f;  //actually, with fish size increasing, mass and force should both increase, but to simplify calculation,
    //we just keep force constant, so mass increase means acceleration decrease
};
struct Boids {};   //mark for boids system, require Entity Type
struct NetworkPeer {
    ENetPeer* peer = nullptr;
};
struct HP {
    float hp { 0.f };  //client will only receive std::uint16 netHP
    float maxHp { 0.f };
    float hpIncRate { 0.f };
};
struct FP {  //food points
    float fp { 0.f };  //same
    float maxFp { 0.f };
    float fpDecRate { 0.f };
};
struct ForceLoadChunk {};   //enable aoi in GridBuildSystem
struct NetSyncComp {  //NetworkSyncSystem
    std::uint8_t offset;  //decide the tick
};
struct Collision {
    // std::vector<HitBox> hitBoxes;
};
struct Attack {
    float baseDamage = 10.f;  //already with size scale
    float skillScale = 1.f;  //scale with skill level
};
struct Defence {
    float damageReduction = 0.f;  //flat damage reduction
};
struct Invincibility {  //mark for invincibility
    float cooldown = 0.f;  //remaining duration
};
struct EntityClearTag {};  //mark for clearing entity should be cleared
struct FoodBall {
    float nutrition{};  //how much fp it can restore
    float lifetime{300.f};  //auto disappear after time
};
struct NoNetControl {  //for player respawn interval
    float countDown = 4.f;
};
struct NameTag {
    std::array<char, 16> name{};
    NameTag() = default;
    explicit NameTag(const char src[16]) {
        std::memcpy(name.data(), src, 16);
    }
};
struct Chase {
    bool isChasing;
    float chaseRadius;
    Entity entity;
};
inline Transform::Transform(UVector vec): x(vec.x), y(vec.y) {};
inline Velocity::Velocity(UVector vec): vx(vec.x), vy(vec.y) {};
inline Force::Force(UVector vec): ax(vec.x), ay(vec.y) {};
inline void Transform::operator+=(const UVector vec) {
    x += vec.x;
    y += vec.y;
}
inline void Velocity::operator+=(const UVector vec) {
    vx += vec.x;
    vy += vec.y;
}
inline void Force::operator+=(const UVector vec) {
    ax += vec.x;
    ay += vec.y;
}
#endif //UNDEROCEAN_COMPONENTS_H
