
#include "client/GameClient.h"
#include "common/net(depricate)/enet.h"
#include "server/GameServer.h"
#include "server/core(deprecate)/GameData.h"
//#define SERVER_ENV
void startNetServer();
int main() {
    enet_initialize();
    //GameData::loadSettings();
#ifdef SERVER_ENV
    GameServer server;
    server.run();
#else
    startNetServer();
    GameClient client;
    client.run();
#endif
    enet_deinitialize();
}
void startNetServer() {
    std::thread serverThread([]() {
        auto server = std::make_unique<GameServer>();
        server->run();
    });
    serverThread.detach();
}
