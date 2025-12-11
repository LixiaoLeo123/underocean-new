//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_COMPONENTS_H
#define UNDEROCEAN_COMPONENTS_H
#include "common/Types.h"
struct UVector;
struct Transform {
    float x = 0, y = 0;
    Transform(UVector vec);
    explicit Transform(float x = 0.f, float y = 0.f) : x(x), y(y) {}
    void operator+=(UVector vec);
};
struct Velocity {
    float vx = 0.f, vy = 0.f;
    Velocity(UVector vec);
    explicit Velocity(float vx = 0.f, float vy = 0.f) : vx(vx), vy(vy) {}
    void operator+=(UVector vec);
};
struct Force {
    float ax = 0.f, ay = 0.f;
    Force(UVector vec);
    explicit Force(float ax = 0.f, float ay = 0.f) : ax(ax), ay(ay) {}
    void operator+=(UVector vec);
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
};
struct FP {  //food points
    float fp { 0.f };  //same
    float maxFp { 0.f };
};
struct ForceLoadChunk {};   //enable aoi in GridBuildSystem
struct NetSyncComp {  //NetworkSyncSystem
    std::uint8_t offset;  //decide the tick
};
struct UVector {   //not a component, just for vector calculation, U for unified
    float x;
    float y;
    constexpr UVector(float x = 0.f, float y = 0.f) : x(x), y(y) {}
    UVector(Transform transform): x(transform.x), y(transform.y) {}
    UVector(Velocity velocity) : x(velocity.vx), y(velocity.vy) {}
    UVector(Force acceleration) : x(acceleration.ax), y(acceleration.ay) {}
    UVector operator+(const UVector other) const {
        return UVector{x + other.x, y + other.y};
    }
    UVector operator-(const UVector other) const {
        return UVector{x - other.x, y - other.y};
    }
    UVector operator*(const UVector other) const {   //for dot
        return UVector{x * other.x, y * other.y};
    }
    UVector operator*(const float scale) const {
        return UVector{x * scale, y * scale};
    }
    UVector operator/(const float scale) const {
        return UVector{x / scale, y / scale};
    }
    void operator*=(const float scale) {
        x *= scale;
        y *= scale;
    }
    void operator/=(const float scale) {
        x /= scale;
        y /= scale;
    }
    void operator+=(const UVector vec) {
        x += vec.x;
        y += vec.y;
    }
    void operator-=(const UVector vec) {
        x -= vec.x;
        y -= vec.y;
    }
    float len() const {
        return sqrt(x * x + y * y);
    }
    float len2() const {
        return (x * x + y * y);
    }
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
