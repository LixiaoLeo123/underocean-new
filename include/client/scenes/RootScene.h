//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_ROOTSCENE_H
#define UNDEROCEAN_ROOTSCENE_H
#include <SFML/Graphics/Font.hpp>
#include "client/common/INodeScene.h"
class SmoothTextLabel;
class RootScene final : public INodeScene{  //the only scene instance!
private:
    static sf::Event dispatchEvent(sf::RenderWindow& window, const sf::Event& rawEvent);
    sf::Font font0_;
    std::shared_ptr<SmoothTextLabel> title_;
public:
    explicit RootScene(sf::RenderWindow& window);
    void handleEventWithDispatch(sf::RenderWindow& window, const sf::Event& rawEvent) {  //world axis convertion in it
        assert(!scenes_.empty());
        sf::Event worldEvent = dispatchEvent(window, rawEvent);
        scenes_.back()->handleEvent(worldEvent);
    }
};
#endif //UNDEROCEAN_ROOTSCENE_H