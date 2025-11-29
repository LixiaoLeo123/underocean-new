//
// Created by 15201 on 11/19/2025.
//

#ifndef UNDEROCEAN_LEVELSELECTMENU_H
#define UNDEROCEAN_LEVELSELECTMENU_H
#include "StatusIndicator.h"
#include "client/common/ResourceManager.h"
#include "client/scenes/FloatingObj.h"
#include "client/scenes/LazyPanelScene.h"
#include "client/ui/widgets/ImageButton.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "common/network/ClientNetworkDriver.h"
#include "server/core/GameData.h"
#define LEVEL_BUTTON_SIZE sf::Vector2f(WIDTH * 0.3f,HEIGHT * 0.35f)
class LevelSelectMenu : public LazyPanelScene {  //network start, real client
public:
    LevelSelectMenu(const std::shared_ptr<SmoothTextLabel>& title, std::string ip, int port);
    void update(float dt) override;
    void whenViewInit(const sf::View &view) override {
        sf::Vector2f titleSize = title_->calculateSizeByWidth(WIDTH / 3.f);
        title_->convertTo(view);                //to cancel the pos bias due to the change of view
        title_->setBounds(sf::Vector2f((WIDTH - titleSize.x) / 2.f, HEIGHT / 150.f), titleSize);
        add(title_);
        statusIndicator_.adjustBound(view);
    }
    void render(sf::RenderWindow &window) override;
    void handleEvent(const sf::Event &event) override;
    void handleConnect();  //callback
    void handleDisconnect();
    void handleLevelButtonClick(int levelNum);   //callback
private:
    std::shared_ptr<SmoothTextLabel> title_;
    sf::Sprite background_;
    FloatingObj obj0_;   //1-3 then cloud, 4-6 then white points
    FloatingObj obj1_;   //1-3 then sea, 4-6 then big ball
    StatusIndicator statusIndicator_ {};  //connected?
    bool advanced{ false };  //GameData::level over 3?
    std::array<std::shared_ptr<ImageButton>, 6> levelButtons_;
    static constexpr int WIDTH = 576;
    static constexpr int HEIGHT = 324;
    ClientNetworkDriver networkDriver_;
    void reloadUI();   //bg, buttons
};
#endif //UNDEROCEAN_LEVELSELECTMENU_H