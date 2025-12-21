//
// Created by 15201 on 12/17/2025.
//

#ifndef UNDEROCEAN_UVECTOR_H
#define UNDEROCEAN_UVECTOR_H
#include <cmath>
struct UVector {   //not a component, just for vector calculation, U for unified
    float x;
    float y;
    constexpr UVector(float x = 0.f, float y = 0.f) : x(x), y(y) {}
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
    [[nodiscard]] float len() const {
        return std::sqrt(x * x + y * y);
    }
    [[nodiscard]] float len2() const {
        return (x * x + y * y);
    }
    UVector& normalize() {  //NOTE: should check len before calling this
        const float l = len();
        x /= l;
        y /= l;
        return *this;
    }
};
#endif //UNDEROCEAN_UVECTOR_H