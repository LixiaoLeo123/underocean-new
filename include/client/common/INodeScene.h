//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_INODESCENE_H
#define UNDEROCEAN_INODESCENE_H
#include <memory>
#include <cassert>
#include "IScene.h"
class INodeScene : public IScene {  //scene that can include other scenes
public:
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override {
        assert(!scenes_.empty());
        scenes_.back()->handleEvent(event);
    }
    void update(float dt) override;
    template<typename T, typename... Args>
    void pushScene(Args&&... args);
    void popScene() {
        assert(!scenes_.empty());
        scenes_.pop_back();
    }
    virtual void handleSwitchRequest(SceneSwitchRequest& req);
protected:
    std::vector<std::unique_ptr<IScene>> scenes_;
};
template<typename T, typename ...Args>
void INodeScene::pushScene(Args && ...args) {
    auto callback = [this](SceneSwitchRequest& req) {
        handleSwitchRequest(req);
    };
    std::unique_ptr<T> scene = std::make_unique<T>(std::forward<Args>(args)...);
    scene->setSwitchCallback(std::move(callback));
    scenes_.push_back(std::move(scene));
}

#endif //UNDEROCEAN_INODESCENE_H