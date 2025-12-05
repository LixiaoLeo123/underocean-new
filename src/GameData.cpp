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
    op(stream, COLLIDER_TYPE);
    op(stream, TPS);
    op(stream, FPS);
    op(stream, IPS);
    op(stream, SERVER_TPS);
    op(stream, ENTITY_SYNC_RADIUS_X);
    op(stream, ENTITY_SYNC_RADIUS_Y);
    op(stream, CAMERA_ALPHA);
    op(stream, playerId);
    op(stream, currentLevel);
    op(stream, SERVER_IP);
    op(stream, SERVER_PORT);
}
void GameData::resetSettings() {   //no file change
    TPS = 40;    //ticks per second on client
    FPS = 60;    //frames per second
    IPS = 20;   //input read per second(see inputmanager::update())
    SERVER_TPS = 40;   //ticks per second on server/game
    ENTITY_SYNC_RADIUS_X = 2;   //NetworkSyncSystem
    ENTITY_SYNC_RADIUS_Y = 1;
    CAMERA_ALPHA = 0.02f;
    playerId[0] = '@';   //@ means not set yet
    currentLevel = 6;
    std::strncpy(SERVER_IP, "127.0.0.1", sizeof(SERVER_IP) - 1);  //only used by client
    SERVER_PORT = 51015;
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