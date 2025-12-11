//
// Created by 15201 on 12/10/2025.
//

#ifndef UNDEROCEAN_PLAYERSTATUS_H
#define UNDEROCEAN_PLAYERSTATUS_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>

class PlayerStatus {  //including HP and FP
public:
    PlayerStatus();
    void render(sf::RenderWindow& window);
    void onWindowSizeChange(unsigned newWidth, unsigned newHeight) {
        float windowRatio = static_cast<float>(newHeight) / static_cast<float>(newWidth);
        uiView.setSize(UI_VIEW_WIDTH, UI_VIEW_WIDTH * windowRatio);
    }
    void update(float dt);
    void setHP(float clampedHp);
    void setMaxHP(float maxHP);
    void setFP(float fp);
    void setMaxFP(float maxFP);
private:
    static constexpr float statusBarWidthRatio = 0.3f;  //relative to window width, single heart
    static constexpr float UI_VIEW_WIDTH = 128.f;
    static constexpr float HEART_SIZE = 5.f;
    static constexpr float FOOD_SIZE = 3.f;
    static constexpr float HP_PER_HEART = 2.f;
    static constexpr float FP_PER_FOOD = 2.f;
    static constexpr float WHITE_HEART_ELAPSE_TIME = 0.25f;
    static constexpr float JUMP_HEART_ELAPSE_TIME = 0.2f;
    float whiteHeartAnimTimer_{0.f};
    bool isWhiteHeartAnim_{false};
    float jumpingHeartAnimTimer_{0.f};
    int jumpingHeartIndex_{-1};  //-1 means no jumping, jumping when HP increased
    bool isHeartJumping_{false};
    std::vector<int> heartStates_{};
    std::vector<int> foodStates_{};
    sf::View uiView {};
    sf::Sprite hpSprites[6];  //six textures
    sf::Sprite fpSprites[6];
    bool hasWindowRatioInit_{false};
    float hp_{0.f};
    float maxHP_{0.f};
    float fp_{0.f};
    float maxFP_{0.f};
};
inline void PlayerStatus::update(float dt) {
    if (isHeartJumping_) {
        jumpingHeartAnimTimer_ += dt;
        if (jumpingHeartAnimTimer_ > JUMP_HEART_ELAPSE_TIME) {
            jumpingHeartAnimTimer_ = 0.f;
            ++jumpingHeartIndex_;
            if (jumpingHeartIndex_ >= heartStates_.size()) {  //finished
                jumpingHeartIndex_ = 0;
                isHeartJumping_ = false;
            }
        }
    }
    if (isWhiteHeartAnim_) {
        whiteHeartAnimTimer_ += dt;
        if (whiteHeartAnimTimer_ > WHITE_HEART_ELAPSE_TIME) {
            whiteHeartAnimTimer_ = 0.f;
            isWhiteHeartAnim_ = false;
            for (int& heartState : heartStates_) {
                if (heartState == 2 || heartState == 1)  //white heart
                    heartState = 0;  //empty heart
                else if (heartState == 4)  //half white heart
                    heartState = 3;  //half heart
            }
        }
    }
}
inline void PlayerStatus::setMaxHP(float maxHP) {
    size_t currentFullHearts = std::floor(hp_ / HP_PER_HEART);
    if (heartStates_.size() == currentFullHearts) return;
    size_t missedHearts = currentFullHearts - heartStates_.size();
    for (size_t i = 0; i < missedHearts; ++i) {
        heartStates_.push_back(0);  //empty
    }
    maxHP_ = maxHP;
    setHP(hp_);  //to update the heart states, preventing bad net packet
}
inline void PlayerStatus::setMaxFP(float maxFP) {
    size_t currentFullFoods = std::floor(fp_ / FP_PER_FOOD);
    if (foodStates_.size() == currentFullFoods) return;
    size_t missedFoods = currentFullFoods - foodStates_.size();
    for (size_t i = 0; i < missedFoods; ++i) {
        foodStates_.push_back(0);  //empty
    }
    maxFP_ = maxFP;
    setFP(fp_);  //to update the food states, preventing bad net packet
}
inline void PlayerStatus::setHP(float hp) {
    float clampedHp = std::min(hp, maxHP_);  //prevent bad net packet causing out of range
    int previousHalfHearts = static_cast<int>(std::round(hp_ / maxHP_));
    int currentHalfHearts = static_cast<int>(std::round(clampedHp / maxHP_));
    if (previousHalfHearts == currentHalfHearts) return;
    hp_ = hp;
    if (currentHalfHearts > previousHalfHearts) {
        jumpingHeartIndex_ = 0;
        jumpingHeartAnimTimer_ = 0.f;
        isHeartJumping_ = true;
    }
    else {
        int fullHearts = currentHalfHearts / 2;
        int halfHeart = currentHalfHearts % 2;
        int prevFullHearts = previousHalfHearts / 2;
        int prevHalfHeart = previousHalfHearts % 2;
        for (size_t i = 0; i < heartStates_.size(); ++i) {
            if (i < fullHearts)
                heartStates_[i] = 5;  //full heart 22
            else if (i == fullHearts)
                if (halfHeart)
                    heartStates_[i] = 4;  //half white heart 21
                else
                    heartStates_[i] = 2;
            else if (i > fullHearts) {
                if (i < prevFullHearts)
                    heartStates_[i] = 2;  //white heart 11
                else if (i == prevFullHearts) {
                    if (prevHalfHeart)
                        heartStates_[i] = 1;  //half empty heart 1
                    else
                        heartStates_[i] = 0;  //empty heart 0
                }
                else
                    heartStates_[i] = 0;  //empty heart 0
            }
        }
        whiteHeartAnimTimer_ = 0.f;
        isWhiteHeartAnim_ = true;
    }
}
inline void PlayerStatus::setFP(float fp) {
    float clampedFp = std::min(fp, maxFP_);
    int previousHalfFoods = static_cast<int>(std::round(fp_ / maxFP_));
    int currentHalfFoods = static_cast<int>(std::round(clampedFp / maxFP_));
    if (previousHalfFoods == currentHalfFoods) return;
    fp_ = fp;
    int fullFoods = currentHalfFoods / 2;
    int halfFood = currentHalfFoods % 2;
    for (size_t i = 0; i < foodStates_.size(); ++i) {
        if (i < fullFoods)
            foodStates_[i] = 5;  //full food 22
        else if (i == fullFoods) {
            if (halfFood)
                foodStates_[i] = 4;  //half food 21
            else
                foodStates_[i] = 2;  //empty food 0
        }
        else
            foodStates_[i] = 0;  //empty food 0
    }
}
inline void PlayerStatus::render(sf::RenderWindow& window) {
    if (!hasWindowRatioInit_) {
        onWindowSizeChange(window.getSize().x, window.getSize().y);
        hasWindowRatioInit_ = true;
    }
    sf::View previousView = window.getView();

}
#endif //UNDEROCEAN_PLAYERSTATUS_H