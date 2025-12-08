//
// Created by 15201 on 11/18/2025.
//


#include "server/core/GameData.h"
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <iostream>

template<typename Stream, typename Op>
void GameData::processSettings(Stream& stream, Op op) {
#define X(type, name, default_val) op(stream, name);
#define X_ARRAY(type, name, size, default_val) op(stream, name);
    GAMEDATA_CONFIG_ENTRIES
#undef X_ARRAY
#undef X
}
void GameData::resetSettings() {   //no file change
#define X(type, name, default_val) name = default_val;
#define X_ARRAY(type, name, size, default_val) \
std::strncpy(name, default_val, size - 1); \
name[size - 1] = '\0';
    GAMEDATA_CONFIG_ENTRIES
#undef X
#undef X_ARRAY
}
void GameData::loadSettings() {    //if file not exist then create a new
    std::ifstream file("settings.cfg", std::ios::binary);
    if (!file) {
        saveSettings();
        return;
    }
    processSettings(file, [](auto& s, auto& var) {
        s.read(reinterpret_cast<char*>(&var), sizeof(var));
        });
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
    processSettings(file, [](auto& s, auto& var) {
        s.write(reinterpret_cast<char*>(&var), sizeof(var));
        });
}