//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_GAMEDATA_H
#define UNDEROCEAN_GAMEDATA_H
#include <string>

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
    inline static ColliderType COLLIDER_TYPE = OBB;
    inline static int TPS = 40;    //ticks per second on client
    inline static int FPS = 60;    //frames per second
    inline static int IPS = 20;   //input read per second(see inputmanager::update())
    inline static int SERVER_TPS = 40;   //ticks per second on server/game
    inline static int ENTITY_SYNC_RADIUS_X = 2;   //NetworkSyncSystem
    inline static int ENTITY_SYNC_RADIUS_Y = 1;   //16 : 9
    inline static float CAMERA_ALPHA = 0.02f;
    inline static char playerId[16] = "@";   //@ means not set yet
    inline static int currentLevel = 6;
    inline static char SERVER_IP[16] = "127.0.0.1";  //only used by client
    inline static int SERVER_PORT = 51015;
private:
    template<typename Stream, typename Op>
    static void processSettings(Stream& stream, Op op);
};
#endif //UNDEROCEAN_GAMEDATA_H