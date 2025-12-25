//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_GAMEDATA_H
#define UNDEROCEAN_GAMEDATA_H
#include "common/Types.h"
#define INIT_PLAYER_SIZE {}
#define INIT_PLAYER_HP   {}
#define INIT_PLAYER_FP   {}
#define INIT_PLAYER_CHARACTER_UNLOCKED {}
#define GAMEDATA_CONFIG_ENTRIES \
X(ColliderType, COLLIDER_TYPE, OBB) \
X(int, TPS, 60) \
X(int, FPS, 60) \
X(int, IPS, 20) \
X(int, SERVER_TPS, 20) \
X(int, ENTITY_SYNC_RADIUS_X, 3) \
X(int, ENTITY_SYNC_RADIUS_Y, 2) \
X(float, CAMERA_ALPHA, 0.05f) \
X_ARRAY(char, playerId, 16, "Ass") \
X_ARRAY(std::uint8_t, playerUUID, 16, {0}) \
X(int, currentLevel, 6) \
X(bool, firstPlay, true) \
X(int, playerType, static_cast<int>(EntityTypeID::SMALL_YELLOW)) \
X_ARRAY(bool, characterUnlocked, static_cast<unsigned long long>(EntityTypeID::COUNT), INIT_PLAYER_CHARACTER_UNLOCKED) \
X_ARRAY(float, playerSize, static_cast<unsigned long long>(EntityTypeID::COUNT), INIT_PLAYER_SIZE) \
X_ARRAY(float, playerHP, static_cast<unsigned long long>(EntityTypeID::COUNT), INIT_PLAYER_HP) \
X_ARRAY(float, playerFP, static_cast<unsigned long long>(EntityTypeID::COUNT), INIT_PLAYER_FP) \
X_ARRAY(int, skillLevel, static_cast<unsigned long long>(EntityTypeID::COUNT) * 4, {0}) \
X_ARRAY(char, SERVER_IP, 16, "127.0.0.1") \
X(int, SERVER_PORT, 51015) \
X_ARRAY(char, REMOTE_SERVER_IP, 16, "111.170.33.13") \
X(int, REMOTE_SERVER_PORT, 11451)
enum ColliderType {
    CIRCLE,
    OBB
};
class GameData {
public:
    static void initSettings();
    static bool loadSettings();    //file to variables
    static void saveSettings();
    static void resetSettings();
    static void applySettings();    //variables to specific settings
    static int getSkillLevel(int index) {
        return skillLevel[static_cast<unsigned long long>(playerType) * 4 + index];
    }
    static bool isCharacterUnlocked(int typeId) {
        if (typeId < 0 || typeId >= static_cast<int>(EntityTypeID::COUNT)) return false;
        return characterUnlocked[typeId];
    }
#define X(type, name, default_val) inline static type name = default_val;
#define X_ARRAY(type, name, size, default_val) inline static type name[size] = default_val;
    GAMEDATA_CONFIG_ENTRIES
#undef X
#undef X_ARRAY
private:
    template<typename Stream, typename Op>
    static void processSettings(Stream& stream, Op op);
};
#undef INIT_PLAYER_SIZE
#undef INIT_PLAYER_HP
#undef INIT_PLAYER_FP
#endif //UNDEROCEAN_GAMEDATA_H