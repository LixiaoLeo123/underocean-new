//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_STARTMENU_H
#define UNDEROCEAN_STARTMENU_H

#include <SFML/Graphics/View.hpp>

#include "client/common/FollowingSprite.h"
#include "client/common/INodeScene.h"
#include "client/ui/widgets/Panel.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "../FloatingObj.h"

namespace sf {
	class Font;
}

class StartMenu : public INodeScene{
public:
	explicit StartMenu(const std::shared_ptr<SmoothTextLabel>& title, bool titleSmooth);  //only logic
	~StartMenu() override = default;
	void render(sf::RenderWindow& window) override;
	void handleEvent(const sf::Event& event) override;
	void update(float dt) override {
		static float totalTime = 0.0f;   //bubble up and down
		totalTime += dt;
		constexpr float amplitude = 10.0f;
		constexpr float frequency = 0.06f;
		constexpr float phase_offset = 1.f;
		bubble0_.update(dt);
		bubble1_.update(dt);
		shark_.update(dt);
		title_->updateTotal(dt);
	}
	void onClickSinglePlayer();
	void onClickMultiPlayer();
	void onClickCharacters();
	void handleSwitchRequest(SceneSwitchRequest &req) override {
		if (req.action == SceneSwitchRequest::None && req.extra == 2) {   //back, reset title
			titleSmooth_ = true;
			viewInit_ = false;  //reset
		}else {
			INodeScene::handleSwitchRequest(req);
		}
	}
private:
	sf::Font& font0_;   //en
	sf::Font& font1_;   //zh-cn
	sf::Sprite background_;    //sea
	sf::Sprite boat_;
	FloatingObj bubble0_;
	FloatingObj bubble1_;
	FollowingSprite shark_;
	sf::Vector2f bubble0BasePos_;
	sf::Vector2f bubble1BasePos_;
	std::shared_ptr<SmoothTextLabel> title_;
	sf::View view_{};   //view area, important for resizing
	Panel selectPanel_;   //singleplayer or multiplayer
	bool viewDirty_ = false;
	bool viewInit_ = false;
	constexpr static int WIDTH = 576;
	constexpr static int HEIGHT = 324;
	bool titleSmooth_{false};  //title anim?
	inline void resetViewArea(unsigned winWidth, unsigned winHeight);
};
inline void StartMenu::resetViewArea(unsigned winWidth, unsigned winHeight) {
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
#endif //UNDEROCEAN_STARTMENU_H