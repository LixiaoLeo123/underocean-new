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
    static constexpr float WHITE_HEART_ELAPSE_TIME = 1.f;
    static constexpr float JUMP_HEART_ELAPSE_TIME = 0.1f;
    float whiteHeartAnimTimer_{0.f};
    bool isWhiteHeartAnim_{false};
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
            setHeartBlack();
        }
        else if (static_cast<int>(std::floor(6.f * whiteHeartAnimTimer_ / WHITE_HEART_ELAPSE_TIME)) % 2) {
            setHeartWhite();
        }
        else {
            setHeartBlack();
        }
    }
    // food animations: same logic as hearts
    if (isFoodJumping_) {
        jumpingFoodAnimTimer_ += dt;
        if (jumpingFoodAnimTimer_ > JUMP_HEART_ELAPSE_TIME) {
            jumpingFoodAnimTimer_ = 0.f;
            ++jumpingFoodIndex_;
            if (jumpingFoodIndex_ >= foodStates_.size()) {  //finished
                jumpingFoodIndex_ = 0;
                isFoodJumping_ = false;
            }
        }
    }
    if (isWhiteFoodAnim_) {
        whiteFoodAnimTimer_ += dt;
        if (whiteFoodAnimTimer_ > WHITE_HEART_ELAPSE_TIME) {
            whiteFoodAnimTimer_ = 0.f;
            isWhiteFoodAnim_ = false;
            for (int& foodState : foodStates_) {
                if (foodState == 2 || foodState == 1)  //white food
                    foodState = 0;  //empty food
                else if (foodState == 4)  //half white food
                    foodState = 3;  //half food
            }
        }
    }
}
inline void PlayerStatus::setHeartWhite() {  //assume that heart decrease
    for (size_t i = 0; i < heartStates_.size(); ++i) {
        if (i < tempNowHalfHearts_ / 2)
            heartStates_[i] = 2;  //full heart 02
        else if (i == tempNowHalfHearts_ / 2) {
            if (tempNowHalfHearts_ % 2)
                heartStates_[i] = 5;
            else
                heartStates_[i] = 2;
        }
        else if (i > tempNowHalfHearts_ / 2) {
            if (i < tempLastHalfHearts_ / 2)
                heartStates_[i] = 5;
            else if (i == tempLastHalfHearts_ / 2) {
                if (tempLastHalfHearts_ % 2)
                    heartStates_[i] = 3;
                else
                    heartStates_[i] = 0;
            } else
                heartStates_[i] = 0;
        }
    }
}
inline void PlayerStatus::setHeartBlack() {
    for (size_t i = 0; i < heartStates_.size(); ++i) {
        if (i < tempNowHalfHearts_ / 2)
            heartStates_[i] = 2;  //full heart 22
        else if (i == tempNowHalfHearts_ / 2) {
            if (tempNowHalfHearts_ % 2)
                heartStates_[i] = 1;  //half white heart 21
            else
                heartStates_[i] = 0;
        }
        else
            heartStates_[i] = 0;
    }
}
inline void PlayerStatus::setMaxHP(float maxHP) {
    size_t currentFullHearts = std::floor(maxHP / HP_PER_HEART);
    if (heartStates_.size() == currentFullHearts) return;
    size_t missedHearts = currentFullHearts - heartStates_.size();
    for (size_t i = 0; i < missedHearts; ++i) {
        heartStates_.push_back(0);  //empty
    }
    maxHP_ = maxHP;
    setHP(hp_);  //to update the heart states, preventing bad net packet
}
inline void PlayerStatus::setMaxFP(float maxFP) {
    // mirror setMaxHP behavior for foods
    size_t currentFullFoods = std::floor(maxFP / FP_PER_FOOD);
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
    int previousHalfHearts = static_cast<int>(std::round(2.f * hp_));
    int currentHalfHearts = static_cast<int>(std::round(2.f * clampedHp));
    if (previousHalfHearts == currentHalfHearts) return;
    hp_ = hp;
    tempLastHalfHearts_ = previousHalfHearts;
    tempNowHalfHearts_ = currentHalfHearts;
    if (currentHalfHearts > previousHalfHearts) {
        jumpingHeartIndex_ = 0;
        jumpingHeartAnimTimer_ = 0.f;
        isHeartJumping_ = true;
        setHeartBlack();
    }
    else {
        tempLastHalfHearts_ = previousHalfHearts;
        setHeartWhite();
        whiteHeartAnimTimer_ = 0.f;
        isWhiteHeartAnim_ = true;
    }
}
inline void PlayerStatus::setFP(float fp) {
    // mirror setHP behavior for foods so foods and hearts share same logic/animation
    float clampedFp = std::min(fp, maxFP_);
    int previousHalfFoods = static_cast<int>(std::round(2.f * fp_));
    int currentHalfFoods = static_cast<int>(std::round(2.f * clampedFp));
    if (previousHalfFoods == currentHalfFoods) return;
    fp_ = fp;
    if (currentHalfFoods > previousHalfFoods) {
        jumpingFoodIndex_ = 0;
        jumpingFoodAnimTimer_ = 0.f;
        isFoodJumping_ = true;
        for (size_t i = 0; i < foodStates_.size(); ++i) {
            if (i < currentHalfFoods / 2)
                foodStates_[i] = 5;  //full heart 22
            else if (i == currentHalfFoods / 2) {
                if (currentHalfFoods % 2)
                    foodStates_[i] = 3;  //half white heart 21
                else
                    foodStates_[i] = 0;
            }
            else
                foodStates_[i] = 0;
        }
    } else {
        int fullFoods = currentHalfFoods / 2;
        int halfFood = currentHalfFoods % 2;
        int prevFullFoods = previousHalfFoods / 2;
        int prevHalfFood = previousHalfFoods % 2;
        for (size_t i = 0; i < foodStates_.size(); ++i) {
            if (i < fullFoods)
                foodStates_[i] = 5;  //full food 22
            else if (i == fullFoods) {
                if (halfFood)
                    foodStates_[i] = 4;  //half white food 21
                else
                    foodStates_[i] = 2;
            }
            else {
                if (i < prevFullFoods)
                    foodStates_[i] = 2;  //white food 11
                else if (i == prevFullFoods) {
                    if (prevHalfFood)
                        foodStates_[i] = 1;  //half empty food 1
                    else
                        foodStates_[i] = 0;  //empty food 0
                } else
                    foodStates_[i] = 0;  //empty food 0
            }
        }
        whiteFoodAnimTimer_ = 0.f;
        isWhiteFoodAnim_ = true;
    }
}
inline void PlayerStatus::render(sf::RenderWindow& window) {
    if (!hasWindowRatioInit_) {
        onWindowSizeChange(window.getSize().x, window.getSize().y);
        hasWindowRatioInit_ = true;
    }
    sf::View previousView = window.getView();
    window.setView(uiView);
    for (size_t i = 0; i < heartStates_.size(); ++i) {
        sf::Sprite& heartSprite = hpSprites[heartStates_[i]];
        heartSprite.setPosition(
                (HEART_SIZE * ICON_BOX_SCALE) * (0.5f + i),
                HEART_SIZE * ICON_BOX_SCALE / 2.f
        );
        float shakeOffset = HEART_SIZE * 0.6f * std::pow((1 - std::min(hp_, maxHP_) / maxHP_), 3.f);  //shake when hp low
        heartSprite.move(Random::fastRandFloat(0, shakeOffset),
            Random::fastRandFloat(0, shakeOffset));
        if (isHeartJumping_ && static_cast<int>(i) == jumpingHeartIndex_) {
            heartSprite.move(0.f, -HEART_SIZE * 0.25f);
        }
        window.draw(heartSprite);
    }
    for (size_t i = 0; i < foodStates_.size(); ++i) {
        sf::Sprite& foodSprite = fpSprites[foodStates_[i]];
        foodSprite.setPosition(
                (FOOD_SIZE * ICON_BOX_SCALE) * (0.5f + i),
                HEART_SIZE * ICON_BOX_SCALE + FOOD_SIZE * ICON_BOX_SCALE / 2.f
        );
        float shakeOffset = FOOD_SIZE * 0.6f * std::pow((1 - std::min(fp_, maxFP_) / maxFP_), 3.f);  //shake when hp low
        foodSprite.move(Random::fastRandFloat(0, shakeOffset),
            Random::fastRandFloat(0, shakeOffset));
        if (isFoodJumping_ && static_cast<int>(i) == jumpingFoodIndex_) {
            foodSprite.move(0.f, -FOOD_SIZE * 0.25f);
        }
        window.draw(foodSprite);
    }
    window.setView(previousView);
}
#endif //UNDEROCEAN_PLAYERSTATUS_H