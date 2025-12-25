
#include <SFML/Graphics/Sprite.hpp>

#include "client/GameClient.h"
#include "client/common/ResourceManager.h"
#include "common/net(depricate)/enet.h"
#include "common/utils/Random.h"
#include "server/GameServer.h"
#include "server/core(deprecate)/GameData.h"
//#define SERVER_ENV
void detachLocalServer();
void showAnnoyingDog();
int main(int argc, char* argv[]) {
    enet_initialize();
    if (!GameData::loadSettings()) {
        showAnnoyingDog();
        return 0;
    }
#ifdef SERVER_ENV
    int port = 51015;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-port" && i + 1 < argc) {
            port = std::atoi(argv[i + 1]);
            if (port <= 0 || port > 65535) {
                std::cerr << "Invalid port: " << argv[i + 1] << ". Using default port " << port << ".\n";
                port = 3000;
            }
            break;
        }
    }
    std::cout << "Starting server on port: " << port << "\n";
    GameServer server(true, port);
    server.run();
#else
    ResourceManager::preload();
    detachLocalServer();
    GameClient client;
    client.run();
#endif
    enet_deinitialize();
}
void detachLocalServer() {
    std::thread serverThread([]() {
        auto server = std::make_unique<GameServer>(false);
        server->run();
    });
    serverThread.detach();
}
void showAnnoyingDog() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "UnderCorrupted", sf::Style::Default);
    window.setFramerateLimit(GameData::FPS);
    window.setVerticalSyncEnabled(true);
    sf::Sprite dog;
    dog.setTexture(ResourceManager::getTexture("images/others/dog.png"));
    dog.setTextureRect({0, 0, 25, 19});
    dog.setOrigin({12.5f, 9.5f});
    dog.setPosition(window.getSize().x / 2u, window.getSize().y / 2u);
    dog.setScale(window.getSize().x / 72.f, window.getSize().x / 72.f);
    sf::Clock animClock{};
    static constexpr float animTime = 0.25f;
    AudioManager::getInstance().playMusic("audio/m_crash.wav");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                window.setView(sf::View({0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height)}));
                dog.setScale(window.getSize().x / 72.f, window.getSize().x / 72.f);
            }
        }
        if (animClock.getElapsedTime().asSeconds() >= animTime) {
            animClock.restart();
            static int frame = 0;
            frame = (frame + 1) % 2;
            dog.setTextureRect({frame * 25, 0, 25, 19});
        }
        float shakeMaxOffset = window.getSize().x / 400.f;
        dog.setPosition(window.getSize().x / 2u + Random::randFloat(0, shakeMaxOffset),  window.getSize().y / 2u + Random::randFloat(0, shakeMaxOffset));
        window.clear();
        window.draw(dog);
        window.display();
    }
}