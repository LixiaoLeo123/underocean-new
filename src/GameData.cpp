//
// Created by 15201 on 11/18/2025.
//


#include "server/core/GameData.h"

#include <fstream>
#include <iosfwd>
#include <iostream>

#include "common/utils/Physics_deprecate.h"

template<typename Stream, typename Op>
void GameData::processSettings(Stream& stream, Op op) {
    op(stream, COLLIDER_TYPE);
    op(stream, TPS);
    op(stream, FPS);
    op(stream, TARGET_AREA);
    op(stream, SIMULATING_DIST);
    op(stream, FREEMODE_MAX_FISH);
    op(stream, IPS);
    op(stream, playerId);
    op(stream, currentLevel);
    op(stream, SERVER_TPS);
}
void GameData::resetSettings() {   //no file change
    COLLIDER_TYPE = OBB;
    TPS = 60;
    FPS = 60;
    TARGET_AREA = 100 * 200;
    SIMULATING_DIST = 5;
    PERCEPTION_DIST = 1;
    FREEMODE_MAX_FISH = 100;
    IPS = 20;
    playerId[0] = '@';
    currentLevel = 1;
    SERVER_TPS = 40;
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
    switch (COLLIDER_TYPE) {
        case OBB:
            Physics::checkCollision = Physics::checkCollisionByOBB;
            break;
        case CIRCLE:
            Physics::checkCollision = Physics::checkCollisionByCircle;
            break;
    }
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