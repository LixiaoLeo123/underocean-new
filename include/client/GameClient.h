//
// Created by 15201 on 11/23/2025.
//

#ifndef UNDEROCEAN_CLIENT_H
#define UNDEROCEAN_CLIENT_H
#include <SFML/Graphics/RenderWindow.hpp>

#include "common/InputManager.h"
#include "scenes/RootScene.h"
#include "server/core/GameData.h"

class GameClient {
public:
    GameClient() = default;
    void run() {
        sf::RenderWindow window(sf::VideoMode(1920, 1080), "Underocean", sf::Style::Default);
        window.setFramerateLimit(GameData::FPS);
        RootScene sceneManager(window);
        sf::Clock clock;
        float MAX_FRAME_TIME = 4.0f / static_cast<float>(GameData::TPS);
        float TIME_STEP = 1.0f / static_cast<float>(GameData::TPS);
        float INPUT_INTERVAL = 1.0f / static_cast<float>(GameData::IPS);
        while (window.isOpen()) {  //update(), render(), InputManager::update()
            float currentTime = clock.getElapsedTime().asSeconds();
            static float accumulator = 0.0f;
            static float lastTime = currentTime;
            static float lastInputTime = currentTime;
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                else
                    sceneManager.handleEventWithDispatch(window, event);
            }
            float frameTime = currentTime - lastTime;
            lastTime = currentTime;
            if (frameTime > MAX_FRAME_TIME)
                frameTime = MAX_FRAME_TIME;
            accumulator += frameTime;
            while (accumulator >= TIME_STEP) {
                sceneManager.update(TIME_STEP);
                accumulator -= TIME_STEP;
            }
            if (currentTime - lastInputTime >= INPUT_INTERVAL) {
                InputManager::getInstance().update(window);
                lastInputTime = currentTime;
            }
            window.clear();
            sceneManager.render(window);
            window.display();
        }
    }
};
#endif //UNDEROCEAN_CLIENT_H