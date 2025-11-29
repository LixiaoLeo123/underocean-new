#include "server/GameServer.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

#include "server/core/GameData.h"
#include "server/new/levels/Level0.h"

GameServer::GameServer(){
    if (!networkDriver_.listen(GameData::SERVER_PORT)) {
        throw std::runtime_error("Error starting server");
    }
    levels_[0] = std::make_unique<Level0>();
    std::cout << "Server started" << std::endl;
}
