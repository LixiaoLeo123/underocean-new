//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_GAMEDATA_H
#define UNDEROCEAN_GAMEDATA_H
#define GAMEDATA_CONFIG_ENTRIES \
X(ColliderType, COLLIDER_TYPE, OBB) \
X(int, TPS, 40) \
X(int, FPS, 60) \
X(int, IPS, 20) \
X(int, SERVER_TPS, 40) \
X(int, ENTITY_SYNC_RADIUS_X, 10) \
X(int, ENTITY_SYNC_RADIUS_Y, 5) \
X(float, CAMERA_ALPHA, 0.02f) \
X_ARRAY(char, playerId, 16, "@") \
X(int, currentLevel, 6) \
X(int, playerType, static_cast<int>(EntityTypeID::SMALL_YELLOW)) \
X(float, playerSize, 20.f) \
X_ARRAY(char, SERVER_IP, 16, "127.0.0.1") \
X(int, SERVER_PORT, 51015)
#include "common/Types.h"

enum ColliderType {
    CIRCLE,
    OBB
};
class GameData {
public:
    static void loadSettings();    //file to variables
    static void saveSettings();
    static void resetSettings();
    static void applySettings();    //variables to specific settings
#define X(type, name, default_val) inline static type name = default_val;
#define X_ARRAY(type, name, size, default_val) inline static type name[size] = default_val;
    GAMEDATA_CONFIG_ENTRIES
#undef X
#undef X_ARRAY
private:
    template<typename Stream, typename Op>
    static void processSettings(Stream& stream, Op op);
};
#endif //UNDEROCEAN_GAMEDATA_H