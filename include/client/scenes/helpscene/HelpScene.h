#ifndef UNDEROCEAN_HELPSCENE_H
#define UNDEROCEAN_HELPSCENE_H

#include "client/common/INodeScene.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "client/common/ResourceManager.h"
#include "../FloatingObj.h"
#include <vector>
#include <memory>

class HelpScene : public INodeScene {
public:
    explicit HelpScene(const std::shared_ptr<SmoothTextLabel>& title);
    ~HelpScene() override = default;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void handleSwitchRequest(SceneSwitchRequest& req) override;
private:
    struct HelpEntry {
        sf::Text keyText;
        sf::Text descText;
        sf::RectangleShape separatorLine;
        void init(const std::string& key, const std::string& desc, float yPos, float width, float targetHeight);
        void render(sf::RenderWindow& window);
    };
    std::shared_ptr<SmoothTextLabel> title_;
    sf::View view_;
    bool viewInit_ = false;
    bool viewDirty_ = false;
    constexpr static int WIDTH = 576;
    constexpr static int HEIGHT = 324;
    sf::Sprite background_;
    FloatingObj bgObj0_;
    FloatingObj bgObj1_;
    sf::RectangleShape contentBox_;
    std::vector<HelpEntry> entries_;
    sf::Text backButton_;
    bool isBackHovered_ = false;
    sf::Font& font_;
    void resetViewArea(unsigned winWidth, unsigned winHeight);
    void goBack();
};

#endif //UNDEROCEAN_HELPSCENE_H