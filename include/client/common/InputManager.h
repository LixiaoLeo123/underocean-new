//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_INPUTMANAGER_H
#define UNDEROCEAN_INPUTMANAGER_H
#include <unordered_set>
#include <unordered_map>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
namespace sf {
    class RenderWindow;
}

class InputManager{    //only instance
public:
    sf::Vector2f mousePosWorld{};
    sf::Vector2i mousePosWindow{};
    bool mousePressedLeft{false};
    bool mousePressedRight{false};
    std::unordered_map<sf::Keyboard::Key, bool> keyDown;
    InputManager() = default;
    static InputManager& getInstance() {
        static InputManager instance;
        return instance;
    }
    void watchKey(sf::Keyboard::Key key) {
        watchedKeys.insert(key);
        keyDown[key] = false;
    }
    void update(const sf::RenderWindow& window);
    bool isKeyDown(sf::Keyboard::Key key) {
        if (auto it = keyDown.find(key); it != keyDown.end()) return it->second;
        return false;
    }
    InputManager(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager& operator=(InputManager&&) = delete;
private:
    std::unordered_set<sf::Keyboard::Key> watchedKeys; //keys need to be actively updated
};
#endif //UNDEROCEAN_INPUTMANAGER_H