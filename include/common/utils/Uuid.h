//
// Created by 15201 on 12/13/2025.
//

#ifndef UNDEROCEAN_UUID_H
#define UNDEROCEAN_UUID_H
#include <array>
#include <cstdint>
#include <random>
using UUIDv4 = std::array<std::uint8_t, 16>;
class Uuid {
public:
    static UUIDv4 generate();
};
inline UUIDv4 Uuid::generate() {  //RFC 4122 version 4 UUIDv4
    static std::mt19937_64 rng{ std::random_device{}() };
    static std::uniform_int_distribution<std::uint64_t> dist;
    UUIDv4 uuid{};
    std::uint64_t r1 = dist(rng);
    std::uint64_t r2 = dist(rng);
    for (int i = 0; i < 8; ++i)
        uuid[i] = static_cast<std::uint8_t>(r1 >> (i * 8));
    for (int i = 0; i < 8; ++i)
        uuid[8 + i] = static_cast<std::uint8_t>(r2 >> (i * 8));
    uuid[6] = (uuid[6] & 0x0F) | 0x40;  //version
    uuid[8] = (uuid[8] & 0x3F) | 0x80;  //variant
    return uuid;
}
#endif //UNDEROCEAN_UUID_H