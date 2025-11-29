//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_KEYBINDINGMANAGER_H
#define UNDEROCEAN_KEYBINDINGMANAGER_H
#include <array>
#include <SFML/Window/Event.hpp>

class KeyBindingManager {
public:
    enum Action {
        Skip,
        Interact,
        None
    };
    static bool hasAction(Action action, const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            Action triggered = getAction(event.key.code);
            return (triggered == action);
        }
        return false;
    }
private:
    static constexpr std::array<std::pair<sf::Keyboard::Key, Action>, 4> keyBindings = { {
        { sf::Keyboard::Z,      Action::Interact },
        { sf::Keyboard::Enter,  Action::Interact },
        { sf::Keyboard::X,  Action::Skip },
        { sf::Keyboard::RShift,      Action::Skip },
    } };
    static constexpr Action getAction(sf::Keyboard::Key key) {
        for (const auto& binding : keyBindings) {
            if (binding.first == key) {
                return binding.second;
            }
        }
        return None;
    }
};
#endif //UNDEROCEAN_KEYBINDINGMANAGER_H