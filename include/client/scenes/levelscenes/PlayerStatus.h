//
// Created by 15201 on 12/10/2025.
//

#ifndef UNDEROCEAN_PLAYERSTATUS_H
#define UNDEROCEAN_PLAYERSTATUS_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

#include "common/utils/Random.h"

class PlayerStatus {  //including HP and FP
public:
    PlayerStatus();
    void render(sf::RenderWindow& window);
    void onWindowSizeChange(unsigned newWidth, unsigned newHeight) {
        float windowRatio = static_cast<float>(newHeight) / static_cast<float>(newWidth);
        uiView.setSize(UI_VIEW_WIDTH, UI_VIEW_WIDTH * windowRatio);
        uiView.setCenter(uiView.getSize() / 2.f);
    }
    void update(float dt);
    void setHP(float clampedHp);
    void setMaxHP(float maxHP);
    void setFP(float fp);
    void setMaxFP(float maxFP);
    //for anim
    void setHeartWhite();  //white and black change 3 times when hp decrease
    void setHeartBlack();
private:
    static constexpr float UI_VIEW_WIDTH = 128.f;
    static constexpr float HEART_SIZE = 3.f;
    static constexpr float FOOD_SIZE = 2.f;
    static constexpr float ICON_BOX_SCALE = 1.05f;  //scale to make icons separate
    static constexpr float HP_PER_HEART = 2.f;
    static constexpr float FP_PER_FOOD = 2.f;
    static constexpr float WHITE_HEART_ELAPSE_TIME = 0.12f;
    static constexpr float WHITE_FOOD_ELAPSE_TIME = 0.7f;
    static constexpr float JUMP_HEART_ELAPSE_TIME = 0.05f;
    static constexpr float JUMP_FOOD_ELAPSE_TIME = 0.1f;
    float whiteHeartAnimTimer_{0.f};
    bool isWhiteHeartAnim_{false};
    bool heartWhite_{false};
    int remainHeartFlashTimes_{0};
    int tempLastHalfHearts_{0};  //for anim
    int tempNowHalfHearts_{0};
    float jumpingHeartAnimTimer_{0.f};
    int jumpingHeartIndex_{-1};  //-1 means no jumping, jumping when HP increased
    bool isHeartJumping_{false};
    // food should have same logic as hearts:
    float whiteFoodAnimTimer_{0.f};
    bool isWhiteFoodAnim_{false};
    float jumpingFoodAnimTimer_{0.f};
    int jumpingFoodIndex_{-1};  //-1 means no jumping, jumping when FP increased
    bool isFoodJumping_{false};
    std::vector<int> heartStates_{};
    std::vector<int> foodStates_{};
    sf::View uiView {};
    sf::Sprite hpSprites[9];  //eight textures
    sf::Sprite fpSprites[6];
    bool hasWindowRatioInit_{false};
    float hp_{0.f};
    float maxHP_{0.f};
    float fp_{0.f};
    float maxFP_{0.f};
};

#endif //UNDEROCEAN_PLAYERSTATUS_H
