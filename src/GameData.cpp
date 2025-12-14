//
// Created by 15201 on 11/18/2025.
//


#include "server/core(deprecate)/GameData.h"
#include <fstream>
#include <iosfwd>
#include <iostream>

#include "common/utils/Integrity.h"
#include "common/utils/Uuid.h"
namespace {
    struct MemoryStream {
        std::vector<std::uint8_t> data;
        std::size_t read_pos = 0;
        MemoryStream() = default;
        explicit MemoryStream(std::vector<std::uint8_t> buf) : data(std::move(buf)) {}
        void write(const std::uint8_t* p, std::size_t size) {
            data.insert(data.end(), p, p + size);
        }
        bool read(void* ptr, std::size_t size) {
            if (read_pos + size > data.size()) {
                return false;
            }
            std::memcpy(ptr, data.data() + read_pos, size);
            read_pos += size;
            return true;
        }
    };
}
template<typename Stream, typename Op>
void GameData::processSettings(Stream& stream, Op op) {
#define X(type, name, default_val) op(stream, name);
#define X_ARRAY(type, name, size, default_val) op(stream, name);
    GAMEDATA_CONFIG_ENTRIES
#undef X_ARRAY
#undef X
}
void GameData::initSettings() {
#define X(type) playerSize[static_cast<unsigned long long>(EntityTypeID::type)] = ParamTable<EntityTypeID::type>::INIT_SIZE; \
    playerHP[static_cast<unsigned long long>(EntityTypeID::type)] = ParamTable<EntityTypeID::type>::HP_BASE; \
    playerFP[static_cast<unsigned long long>(EntityTypeID::type)] = ParamTable<EntityTypeID::type>::FP_BASE;
    PLAYER_ENTITY_TYPES
#undef X
    UUIDv4 temp = Uuid::generate();
    for (int i = 0; i < 16; ++i) {
        playerUUID[i] = temp[i];
    }
}
void GameData::resetSettings() {   //no file change
// #define X(type, name, default_val) name = default_val;
// #define X_ARRAY(type, name, size, default_val) \
// std::strncpy(name, default_val, size - 1); \
// name[size - 1] = '\0';
//     GAMEDATA_CONFIG_ENTRIES
// #undef X
// #undef X_ARRAY
}
bool GameData::loadSettings() {    //if file not exist then create a new
    std::ifstream file("settings.cfg", std::ios::binary);
    if (!file) {
        initSettings();
        saveSettings();
        return true;
    }
    std::vector<std::uint8_t> fileData(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    if (fileData.size() < 32) return false;  //invalid
    const size_t configSize = fileData.size() - 32;
    std::vector<std::uint8_t> configData(fileData.begin(), fileData.begin() + configSize);
    std::array<std::uint8_t, 32> storedHash;
    std::copy(fileData.end() - 32, fileData.end(), storedHash.data());
    auto computedHash = Integrity::hash(configData.data(), configData.size());
    if (computedHash != storedHash) {
        return false;  //integrity check failed
    }
    Integrity::unprotect(configData.data(), configData.size());
    MemoryStream reader(std::move(configData));
    processSettings(reader, [](auto& s, auto& var) {
        s.read(&var, sizeof(var));
    });
    return true;
}
void GameData::applySettings() {
    // switch (COLLIDER_TYPE) {
    //     case OBB:
    //         Physics::checkCollision = Physics::checkCollisionByOBB;
    //         break;
    //     case CIRCLE:
    //         Physics::checkCollision = Physics::checkCollisionByCircle;
    //         break;
    // }
}
void GameData::saveSettings() {
    std::ofstream file("settings.cfg", std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Failed to open settings.cfg for writing\n";
        return;
    }
    MemoryStream writer;
    processSettings(writer, [](auto& s, auto& var) {
        s.write(reinterpret_cast<std::uint8_t*>(&var), sizeof(var));
        });
    Integrity::protect(writer.data.data(), writer.data.size());
    std::array<std::uint8_t, 32> hash = Integrity::hash(writer.data.data(), writer.data.size());
    file.write(reinterpret_cast<const char*>(writer.data.data()), writer.data.size());
    file.write(reinterpret_cast<const char*>(hash.data()), hash.size());
}