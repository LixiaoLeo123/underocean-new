#include "client/common/InputManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Mouse.hpp>

void InputManager::update(const sf::RenderWindow &window) {
    mousePosWindow = sf::Mouse::getPosition(window);
    mousePosWorld = window.mapPixelToCoords(mousePosWindow);
    if (window.hasFocus()) {
        mousePressedLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        mousePressedRight = sf::Mouse::isButtonPressed(sf::Mouse::Right);
    }
    else {
        mousePressedLeft = mousePressedRight = false;
    }
    for (auto key: watchedKeys) {
        keyDown[key] = sf::Keyboard::isKeyPressed(key);
    }
}
