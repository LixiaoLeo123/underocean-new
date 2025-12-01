//
// Created by 15201 on 11/20/2025.
//

#ifndef UNDEROCEAN_COMPONENTS_H
#define UNDEROCEAN_COMPONENTS_H
struct Transform {
    float x = 0, y = 0;
};
struct Velocity {
    float vx = 0.f, vy = 0.f;
};
struct Acceleration {
    float ax = 0.f, ay = 0.f;
};
struct MaxVelocity {
    float maxVelocity = 10.f;
};
struct MaxAcceleration {
    float maxAcceleration = 10.f;
};
struct EntityType {   //"imagined" bio type, for boids and texture
    enum class ID : std::uint8_t {
        NONE,
        SMALL_YELLOW,
    };
    ID entityID = ID::NONE;
};
#endif //UNDEROCEAN_COMPONENTS_H