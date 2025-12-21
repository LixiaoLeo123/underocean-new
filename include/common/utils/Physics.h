//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_PHYSICS_H
#define UNDEROCEAN_PHYSICS_H
#include <cmath>
#include <span>
#include "server/new/component/Components.h"

class Physics {
public :
    static constexpr float PI = 3.1415926f;
    static bool checkCollisionByOBB(const std::span<const HitBox>& hitBoxesA, const std::span<const HitBox>& hitBoxesB,
                                        const UVector& posA, float rotA, const UVector& posB, float rotB,
                                        float scaleA, float scaleB);
    // static bool checkCollisionByCircle(const OldEntity& a, const OldEntity& b);  //for high performance
    static float dot(const UVector& a, const UVector& b);
    static std::pair<float, float> project(const std::array<UVector, 4>& vertices, const UVector& axis);
    static bool overlap(const std::pair<float, float>& a, const std::pair<float, float>& b);
    static bool obbIntersect(const std::array<UVector, 4>& A, const std::array<UVector, 4>& B);
    static float distance(UVector a, UVector b);
    static float distance2(UVector a, UVector b);
    static UVector makeVec(float angleRad, float len) {
        return {std::cos(angleRad) * len, std::sin(angleRad) * len};
    }
    static UVector makeVec(UVector rawVec, float len) {
        float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
        if (rawLen == 0.f) {
            return {0.f, 0.f};
        }
        return UVector(rawVec) / rawLen * len;
    }
    static UVector clampVec(const UVector& rawVec, float len);
};
inline bool Physics::checkCollisionByOBB(const std::span<const HitBox>& hitBoxesA, const std::span<const HitBox>& hitBoxesB,
                                        const UVector& posA, float rotA, const UVector& posB, float rotB,
                                        float scaleA, float scaleB) {
    for (const auto& boxA : hitBoxesA) {   //by obb
        auto verticesA = boxA.getWorldVertices(posA, rotA, scaleA);
        for (const auto& boxB : hitBoxesB) {
            auto verticesB = boxB.getWorldVertices(posB, rotB, scaleB);
            if (obbIntersect(verticesA, verticesB)) {
                return true;
            }
        }
    }
    return false;
}
inline float Physics::dot(const UVector& a, const UVector& b) {
    return a.x * b.x + a.y * b.y;
}
inline std::pair<float, float> Physics::project(const std::array<UVector, 4>& vertices, const UVector& axis) {
    float min = dot(vertices[0], axis);    //axis len must be 1 to get absolute result
    float max = min;
    for (int i = 1; i < 4; ++i) {
        float d = dot(vertices[i], axis);
        if (d < min) min = d;
        if (d > max) max = d;
    }
    return { min, max };
}
inline bool Physics::overlap(const std::pair<float, float>& a, const std::pair<float, float>& b) {
    return a.first <= b.second && b.first <= a.second;
}
inline bool Physics::obbIntersect(const std::array<UVector, 4>& A, const std::array<UVector, 4>& B) {
    for (int i = 0; i < 2; ++i) {
        UVector edge = { A[i + 1].x - A[i].x, A[i + 1].y - A[i].y };
        UVector axis = { -edge.y, edge.x };
        if (!overlap(Physics::project(A, axis), Physics::project(B, axis)))
            return false;
    }
    for (int i = 0; i < 2; ++i) {
        UVector edge = { B[i + 1].x - B[i].x, B[i + 1].y - B[i].y };
        UVector axis = { -edge.y, edge.x };
        if (!overlap(Physics::project(A, axis), Physics::project(B, axis)))
            return false;
    }
    return true;
}
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
inline UVector Physics::clampVec(const UVector& rawVec, float len) {
    float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
    if (rawLen > len) {
        return UVector(rawVec) / rawLen * len;
    }
    return {rawVec};
}

#endif //UNDEROCEAN_PHYSICS_H