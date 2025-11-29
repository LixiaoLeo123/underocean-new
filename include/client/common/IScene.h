//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_ISCENE_H
#define UNDEROCEAN_ISCENE_H
#include <functional>
#include <memory>
#include <SFML/Window/Event.hpp>
namespace sf {
    class RenderWindow;
}
class IScene;
struct SceneSwitchRequest {
    enum Action { Push, Replace, Pop, None };   //none for extra information
    Action action;
    std::unique_ptr<IScene> newScene;
    int ttl;   //time to live, meaning the needed callback depth in the tree
    int extra;  //for extra information
};
class IScene {
public:
    IScene() = default;
    explicit IScene(std::function<void(SceneSwitchRequest&)>&& callback)
        :onRequestSwitch_(callback) {};  //maybe useless
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual bool shouldUpdateWhenNotActive() { return false; }
    virtual bool shouldRenderWhenNotActive() { return false; }
    void setSwitchCallback(std::function<void(SceneSwitchRequest&)>&& callback) {
        onRequestSwitch_ = callback;
    }
    virtual ~IScene() = default;
protected:
    std::function<void(SceneSwitchRequest&)> onRequestSwitch_;
};
#endif //UNDEROCEAN_ISCENE_H