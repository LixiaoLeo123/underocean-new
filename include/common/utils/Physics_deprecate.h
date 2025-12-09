//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_PHYSICS_H
#define UNDEROCEAN_PHYSICS_H
#include "server/game/OldEntity.h"
#include <functional>

#include "server/core(deprecate)/CollisionBox.h"

class Physics_deprecate {
public:
    static constexpr float PI = 3.1415926f;
    static bool checkCollisionByOBB(const OldEntity& a, const OldEntity& b);
    static bool checkCollisionByCircle(const OldEntity& a, const OldEntity& b);  //for high performance
    static sf::Vector2f getSeparationVector(const OldEntity& a, const OldEntity& b);
    static float dot(const sf::Vector2f& a, const sf::Vector2f& b);
    static std::pair<float, float> project(const std::array<sf::Vector2f, 4>& vertices, const sf::Vector2f& axis);
    static bool overlap(const std::pair<float, float>& a, const std::pair<float, float>& b);
    static bool obbIntersect(const std::array<sf::Vector2f, 4>& A, const std::array<sf::Vector2f, 4>& B);
    static float distance(const sf::Vector2f& a, const sf::Vector2f& b);
    static float distance2(const sf::Vector2f& a, const sf::Vector2f& b);
    static sf::Vector2f clampVec(const sf::Vector2f& rawVec, float len);
    static float lenOf(const sf::Vector2f& target);
    static float len2Of(const sf::Vector2f& target);
    inline static std::function<bool(const OldEntity& a, const OldEntity& b)> checkCollision = checkCollisionByOBB;   //can be set by GameData::applySettings()
};
inline bool Physics::checkCollisionByOBB(const OldEntity& a, const OldEntity& b) {
    sf::Vector2f diff = a.position - b.position;          //by circle
    float distance2 = (diff.x * diff.x + diff.y * diff.y);
    if (distance2 < (a.size + b.size) * (a.size + b.size)) return true;
    for (const auto& boxA : a.boxes) {   //by obb
        auto verticesA = boxA.getWorldVertices(a.position, a.rotation);
        for (const auto& boxB : b.boxes) {
            auto verticesB = boxB.getWorldVertices(b.position, b.rotation);
            if (obbIntersect(verticesA, verticesB)) {
                return true;
            }
        }
    }
    return false;
}
inline bool Physics::checkCollisionByCircle(const OldEntity& a, const OldEntity& b) {
    sf::Vector2f diff = a.position - b.position;          //by circle
    float distance2 = (diff.x * diff.x + diff.y * diff.y);
    if (distance2 < (a.size + b.size) * (a.size + b.size)) return true;
    return false;
}
inline sf::Vector2f Physics::getSeparationVector(const OldEntity& a, const OldEntity& b) {
    sf::Vector2f diff = a.position - b.position;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    float overlap = (a.size + b.size) - distance;
    return diff / distance * overlap;
}
inline float Physics::dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}
inline std::pair<float, float> Physics::project(const std::array<sf::Vector2f, 4>& vertices, const sf::Vector2f& axis) {
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
inline bool Physics::obbIntersect(const std::array<sf::Vector2f, 4>& A, const std::array<sf::Vector2f, 4>& B) {
    for (int i = 0; i < 2; ++i) {
        sf::Vector2f edge = { A[i + 1].x - A[i].x, A[i + 1].y - A[i].y };
        sf::Vector2f axis = { -edge.y, edge.x };
        if (!overlap(Physics::project(A, axis), Physics::project(B, axis)))
            return false;
    }
    for (int i = 0; i < 2; ++i) {
        sf::Vector2f edge = { B[i + 1].x - B[i].x, B[i + 1].y - B[i].y };
        sf::Vector2f axis = { -edge.y, edge.x };
        if (!overlap(Physics::project(A, axis), Physics::project(B, axis)))
            return false;
    }
    return true;
}
inline float Physics::distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    sf::Vector2f diff = a - b;
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}
inline float Physics::distance2(const sf::Vector2f& a, const sf::Vector2f& b) {
    sf::Vector2f diff = a - b;
    return (diff.x * diff.x + diff.y * diff.y);
}
inline sf::Vector2f Physics::clampVec(const sf::Vector2f& rawVec, float len) {
    float rawLen = std::sqrt(rawVec.x * rawVec.x + rawVec.y * rawVec.y);
    if (rawLen > len) {
        return sf::Vector2f(rawVec) / rawLen * len;
    }
    return sf::Vector2f(rawVec);
}
inline float Physics::lenOf(const sf::Vector2f& target) {
    return std::sqrt(target.x * target.x + target.y * target.y);
}
inline float Physics::len2Of(const sf::Vector2f& target) {
    return target.x * target.x + target.y * target.y;
}
#endif //UNDEROCEAN_PHYSICS_H