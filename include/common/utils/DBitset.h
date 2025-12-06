//
// Created by 15201 on 12/5/2025.
//

#ifndef UNDEROCEAN_DBITSET_H
#define UNDEROCEAN_DBITSET_H
#include "common/Types.h"
constexpr size_t WORD_COUNT = (MAX_ENTITIES + 63) / 64; // 1024
//NOTE: only server for this game (Using MAX_ENTITIES)
struct DBitset {  //faster, can iterate by 64-bit chunk
    uint64_t words[WORD_COUNT] = {};
    void set(Entity id) {
        words[id >> 6] |= (1ULL << (id & 63));
    }
    void reset(Entity id) {
        words[id >> 6] &= ~(1ULL << (id & 63));
    }
    void reset() {
        std::fill(std::begin(words), std::end(words), 0ULL);
    }
    [[nodiscard]] bool test(Entity id) const {
        return words[id >> 6] & (1ULL << (id & 63));
    }
    [[nodiscard]] const uint64_t* data() const { return words; }
    DBitset operator~() const {
        DBitset result;
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            result.words[i] = ~words[i];
        }
        return result;
    }

    DBitset operator&(const DBitset& other) const {
        DBitset result;
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            result.words[i] = words[i] & other.words[i];
        }
        return result;
    }
    DBitset operator|(const DBitset& other) const {
        DBitset result;
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            result.words[i] = words[i] | other.words[i];
        }
        return result;
    }
    DBitset operator^(const DBitset& other) const {
        DBitset result;
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            result.words[i] = words[i] ^ other.words[i];
        }
        return result;
    }
    DBitset& operator&=(const DBitset& other) {
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            words[i] &= other.words[i];
        }
        return *this;
    }
    DBitset& operator|=(const DBitset& other) {
        for (size_t i = 0; i < WORD_COUNT; ++i) {
            words[i] |= other.words[i];
        }
        return *this;
    }
};
#endif //UNDEROCEAN_DBITSET_H