//
// Created by 15201 on 12/17/2025.
//

#ifndef UNDEROCEAN_COLLISIONBOX_H
#define UNDEROCEAN_COLLISIONBOX_H
#include <array>

#include "UVector.h"

struct HitBox {  //pos are all relative
    float hwidth;  //half width
    float hheight;
    float offsetX;
    float offsetY;
    float rot;  //radians
    constexpr HitBox(float hwidth, float hheight, float offsetX = 0.f, float offsetY = 0.f, float rot = 0.f)
        : hwidth(hwidth), hheight(hheight), offsetX(offsetX), offsetY(offsetY), rot(rot) {}
    HitBox operator*(float scale) const {
        return {hwidth * scale, hheight * scale, offsetX * scale, offsetY * scale, rot};
    }
    void operator+=(const UVector& pos) {
        offsetX += pos.x;
        offsetY += pos.y;
    }
    [[nodiscard]] std::array<UVector, 4> getWorldVertices(UVector entityPos, float entityRot, float scale) const {
        float totalRot = entityRot + rot;
        float cosR = std::cos(totalRot);
        float sinR = std::sin(totalRot);
        std::array local = {
            UVector(-hwidth, -hheight),
            UVector(hwidth, -hheight),
            UVector(hwidth, hheight),
            UVector(-hwidth, hheight)
        };
        UVector boxCenterWorld(
            entityPos.x + scale * (offsetX * std::cos(entityRot) - offsetY * std::sin(entityRot)),
            entityPos.y - scale * (offsetX * std::sin(entityRot) + offsetY * std::cos(entityRot))
        );
        std::array<UVector, 4> world;  // rotate
        for (int i = 0; i < 4; ++i) {
            UVector p = local[i];
            world[i] = UVector(
                boxCenterWorld.x + scale * (p.x * cosR - p.y * sinR),
                boxCenterWorld.y - scale * (p.x * sinR + p.y * cosR)
            );
        }
        return world;
    }
};
#endif //UNDEROCEAN_COLLISIONBOX_H