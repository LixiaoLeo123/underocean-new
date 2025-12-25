//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_PANELSCENE_H
#define UNDEROCEAN_PANELSCENE_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "client/common/IScene.h"
#include "client/ui/layouts/LazyLayout.h"
#include "client/ui/widgets/Panel.h"

class LazyPanelScene : public IScene {  //scene use panel with lazylayout, manage view actively
public:
    explicit LazyPanelScene(float initWidth = 1920, float initHeight = 1080)
        :panel_(sf::Vector2f(0,0), sf::Vector2f(initWidth, initHeight),
            std::make_unique<LazyLayout>(sf::Vector2f(initWidth, initHeight), sf::Vector2f(0,0))),
        WIDTH(initWidth), HEIGHT(initHeight){
    }
    virtual void whenViewInit(const sf::View& view) = 0;
    void add(const std::shared_ptr<IWidget>& widget) { panel_.add(widget); }  //add component
    void render(sf::RenderWindow &window) override {
        if (!viewInit_) {   //render area init
            sf::Vector2u winSize(window.getSize());
            resetViewArea(winSize.x, winSize.y);
            whenViewInit(view_);  //need to complete
            panel_.setBounds(view_.getCenter() - view_.getSize() / 2.f, view_.getSize());
            window.setView(view_);
            viewInit_ = true;
        }
        if (viewDirty_) {
            viewDirty_ = false;
            window.setView(view_);
        }
        //do other things here
    }
    void renderPanel(sf::RenderWindow &window) { panel_.render(window); }
    Panel& getPanel() { return panel_; }
    void handleEvent(const sf::Event &event) override {
        if (event.type == sf::Event::Resized) {
            unsigned int winWidth = event.size.width;
            unsigned int winHeight = event.size.height;
            resetViewArea(winWidth, winHeight);
            panel_.setBounds(view_.getCenter() - view_.getSize() / 2.f, view_.getSize());
        }
    }

private:
    Panel panel_;
    bool viewDirty_ = false;
    float WIDTH;
    float HEIGHT;
    inline void resetViewArea(unsigned winWidth, unsigned winHeight);
protected:
    sf::View view_;
    bool viewInit_ = false;
};
inline void LazyPanelScene::resetViewArea(unsigned winWidth, unsigned winHeight) {
    float windowRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);
    sf::Vector2f viewSize;
    if (windowRatio < static_cast<float>(WIDTH) / HEIGHT) {
        viewSize = sf::Vector2f(HEIGHT * windowRatio, HEIGHT);
    }
    else {
        viewSize = sf::Vector2f(WIDTH, WIDTH / windowRatio);
    }
    view_.reset(sf::FloatRect(0, 0, viewSize.x, viewSize.y));
    view_.setCenter(WIDTH / 2.f, HEIGHT / 2.f);
    viewDirty_ = true;
}
#endif //UNDEROCEAN_PANELSCENE_H