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
    inline static int TARGET_AREA = 200 * 100;   //view area
    inline static int SIMULATING_DIST = 2;  //radius by chunk (about 50px x 50px)
    inline static int PERCEPTION_DIST = 1;  //radius by chunk which fish can get access to
    inline static int FREEMODE_MAX_FISH = 10000;
    inline static float CAMERA_ALPHA = 0.02f;
    inline static int IPS = 20;   //input read per second(see inputmanager::update())
    inline static char playerId[16] = "@";   //@ means not set yet
    inline static int currentLevel = 6;
    inline static int SERVER_TPS = 40;   //ticks per second on server/game
    inline static int SERVER_PORT = 51015;
    inline static char SERVER_IP[16] = "127.0.0.1";  //only used by client
private:
    template<typename Stream, typename Op>
    static void processSettings(Stream& stream, Op op);
};
#endif //UNDEROCEAN_GAMEDATA_H