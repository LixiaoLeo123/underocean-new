//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_PHYSICS_H
#define UNDEROCEAN_PHYSICS_H
#include <cmath>

#include "server/new/component/Components.h"

class Physics {
public :
    static float distance(UVector a, UVector b);
    static float distance2(UVector a, UVector b);
};
inline float Physics::distance(UVector a, UVector b) {
    float diffX = a.x - b.x;
    float diffY = a.y - b.y;
    return std::sqrt(diffX * diffX + diffY * diffY);
}
inline float Physics::distance2(UVector a, UVector b) {
    float diffX = a.x - b.x;
    float diffY = a.y - b.y;
    return (diffX * diffX + diffY * diffY);
}
#endif //UNDEROCEAN_PHYSICS_H