//
// Created by 15201 on 11/18/2025.
//

#include "client/common/INodeScene.h"

#include <iostream>

void INodeScene::render(sf::RenderWindow& window) {
    assert(scenes_.size() > 0);
    for (size_t i = 0; i < scenes_.size(); ++i) {
        IScene* scene = scenes_[i].get();
        if (i == scenes_.size() - 1) {
            scene->render(window);
        }
        else if (scene->shouldRenderWhenNotActive()) {
            scene->render(window);
        }
    }
}
void INodeScene::update(float dt) {
    assert(scenes_.size() > 0);
    for (size_t i = 0; i < scenes_.size(); ++i) {
        IScene* scene = scenes_[i].get();
        if (i == scenes_.size() - 1) {
            scene->update(dt);
        }
        else if (scene->shouldUpdateWhenNotActive()) {
            scene->update(dt);
        }
    }
}
void INodeScene::handleSwitchRequest(SceneSwitchRequest& req) {
    if (req.ttl > 0) {
        --req.ttl;
        onRequestSwitch_(req);
    }
    else {
        if (req.action == SceneSwitchRequest::Pop) {
            if (!scenes_.empty()) scenes_.pop_back();
            return;
        }
        std::unique_ptr<IScene> newScene = std::move(req.newScene);
        auto callback = [this](SceneSwitchRequest& innerReq) {
            handleSwitchRequest(innerReq);
            };
        newScene->setSwitchCallback(callback);
        if (req.action == SceneSwitchRequest::Replace) {
            scenes_.pop_back();
            scenes_.push_back(std::move(newScene));
        }
        else if (req.action == SceneSwitchRequest::Push) {
            scenes_.push_back(std::move(newScene));
        }
    }
}
