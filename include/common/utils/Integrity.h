//
// Created by 15201 on 12/13/2025.
//

#ifndef UNDEROCEAN_INTEGRITY_H
#define UNDEROCEAN_INTEGRITY_H
#include <cstddef>
#include <cstdint>
namespace Integrity {
    void protect(std::uint8_t* data, std::size_t size);
    bool unprotect(std::uint8_t* data, std::size_t size);
    std::array<std::uint8_t, 32> hash(const std::uint8_t* data, std::size_t size);
}
#endif //UNDEROCEAN_INTEGRITY_H