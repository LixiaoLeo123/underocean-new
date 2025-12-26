#include "client/scenes/levelscenes/PlayerStatus.h"

#include "client/common/ResourceManager.h"
#include "server/core(deprecate)/GameData.h"

PlayerStatus::PlayerStatus() {
    uiView.setSize(UI_VIEW_WIDTH, 0.f);
    hpSprites[0].setTexture(ResourceManager::getTexture("images/icons/heart00.png"));
    hpSprites[1].setTexture(ResourceManager::getTexture("images/icons/heart01.png"));
    hpSprites[2].setTexture(ResourceManager::getTexture("images/icons/heart02.png"));
    hpSprites[3].setTexture(ResourceManager::getTexture("images/icons/heart110.png"));
    hpSprites[4].setTexture(ResourceManager::getTexture("images/icons/heart120.png"));
    hpSprites[5].setTexture(ResourceManager::getTexture("images/icons/heart121.png"));
    hpSprites[6].setTexture(ResourceManager::getTexture("images/icons/heart122.png"));
    hpSprites[7].setTexture(ResourceManager::getTexture("images/icons/heart100.png"));
    hpSprites[8].setTexture(ResourceManager::getTexture("images/icons/heart111.png"));
    fpSprites[0].setTexture(ResourceManager::getTexture("images/icons/food00.png"));
    fpSprites[1].setTexture(ResourceManager::getTexture("images/icons/food10.png"));
    fpSprites[2].setTexture(ResourceManager::getTexture("images/icons/food11.png"));
    fpSprites[3].setTexture(ResourceManager::getTexture("images/icons/food20.png"));
    fpSprites[4].setTexture(ResourceManager::getTexture("images/icons/food21.png"));
    fpSprites[5].setTexture(ResourceManager::getTexture("images/icons/food22.png"));
    float heartOriginalSize = static_cast<float>(hpSprites[0].getTexture()->getSize().x);
    float heartScale = HEART_SIZE / heartOriginalSize;
    float foodOriginalSize = static_cast<float>(fpSprites[0].getTexture()->getSize().x);
    float foodScale = FOOD_SIZE / foodOriginalSize;
    for (int i = 0; i < 6; ++i) {
        hpSprites[i].setOrigin(hpSprites[i].getTexture()->getSize().x / 2.f,
            hpSprites[i].getTexture()->getSize().y / 2.f);
        hpSprites[i].setScale(heartScale, heartScale);
        fpSprites[i].setOrigin(fpSprites[i].getTexture()->getSize().x / 2.f,
            fpSprites[i].getTexture()->getSize().y / 2.f);
        fpSprites[i].setScale(foodScale, foodScale);
    }
    for (int i = 6; i < 9; ++i) {
        hpSprites[i].setOrigin(hpSprites[i].getTexture()->getSize().x / 2.f,
            hpSprites[i].getTexture()->getSize().y / 2.f);
        hpSprites[i].setScale(heartScale, heartScale);
    }
}
void PlayerStatus::update(float dt) {
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
            if (remainHeartFlashTimes_ > 0) {
                --remainHeartFlashTimes_;
                if (heartWhite_) {
                    setHeartBlack();
                }
                else {
                    setHeartWhite();
                }
            }
            else {
                setHeartBlack();
                isWhiteHeartAnim_ = false;
            }
        }
    }
    // food animations: same logic as hearts
    if (isFoodJumping_) {
        jumpingFoodAnimTimer_ += dt;
        if (jumpingFoodAnimTimer_ > JUMP_FOOD_ELAPSE_TIME) {
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
        if (whiteFoodAnimTimer_ > WHITE_FOOD_ELAPSE_TIME) {
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
void PlayerStatus::setHeartWhite() {  //assume that heart decrease
    heartWhite_ = true;
    for (size_t i = 0; i < heartStates_.size(); ++i) {
        if (i < tempNowHalfHearts_ / 2)
            heartStates_[i] = 6;  //full heart 02
        else if (i == tempNowHalfHearts_ / 2) {
            if (tempNowHalfHearts_ % 2) {
                if (tempLastHalfHearts_ % 2)
                    heartStates_[i] = 8;
                else
                    heartStates_[i] = 5;
            }
            else {
                if (tempLastHalfHearts_ % 2)
                    heartStates_[i] = 3;
                else if (tempLastHalfHearts_ == tempNowHalfHearts_)
                    heartStates_[i] = 7;
                else
                    heartStates_[i] = 4;
            }
        }
        else if (i > tempNowHalfHearts_ / 2) {
            if (i < tempLastHalfHearts_ / 2)
                heartStates_[i] = 4;
            else if (i == tempLastHalfHearts_ / 2) {
                if (tempLastHalfHearts_ % 2)
                    heartStates_[i] = 3;
                else
                    heartStates_[i] = 7;
            } else
                heartStates_[i] = 7;
        }
    }
}
void PlayerStatus::setHeartBlack() {
    heartWhite_ = false;
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
void PlayerStatus::setMaxHP(float maxHP) {
    size_t currentFullHearts = std::floor(maxHP / HP_PER_HEART);
    if (heartStates_.size() == currentFullHearts) return;
    if (currentFullHearts > heartStates_.size()) {
        size_t missedHearts = currentFullHearts - heartStates_.size();
        for (size_t i = 0; i < missedHearts; ++i) {
            heartStates_.push_back(0);  //empty
        }
    }
    else {
        size_t redundantHearts = heartStates_.size() - currentFullHearts;
        for (int i = 0; i < redundantHearts; ++i) {
            heartStates_.pop_back();
        }
    }
    maxHP_ = maxHP;
    setHP(hp_);  //to update the heart states, preventing bad net packet
}
void PlayerStatus::setMaxFP(float maxFP) {
    // mirror setMaxHP behavior for foods
    size_t currentFullFoods = std::floor(maxFP / FP_PER_FOOD);
    if (foodStates_.size() == currentFullFoods) return;
    if (currentFullFoods > foodStates_.size()) {
        size_t missedFoods = currentFullFoods - foodStates_.size();
        for (size_t i = 0; i < missedFoods; ++i) {
            foodStates_.push_back(0);  //empty
        }
    }
    else {
        size_t redundantFoods = foodStates_.size() - currentFullFoods;
        for (int i = 0; i < redundantFoods; ++i) {
            foodStates_.pop_back();
        }
    }
    maxFP_ = maxFP;
    setFP(fp_);  //to update the food states, preventing bad net packet
}
void PlayerStatus::setHP(float hp) {
    if (hp == hp_) return;
    float clampedHp = std::min(hp, maxHP_);  //prevent bad net packet causing out of range
    int previousHalfHearts = static_cast<int>(std::round(2.f * hp_ / HP_PER_HEART));
    int currentHalfHearts = static_cast<int>(std::round(2.f * clampedHp / HP_PER_HEART));
    hp_ = hp;
    GameData::playerHP[GameData::playerType] = hp_;
    if (!isWhiteHeartAnim_)
        tempLastHalfHearts_ = previousHalfHearts;
    tempNowHalfHearts_ = currentHalfHearts;
    if (currentHalfHearts > previousHalfHearts) {
        jumpingHeartIndex_ = 0;
        jumpingHeartAnimTimer_ = 0.f;
        isHeartJumping_ = true;
        setHeartBlack();
    }
    else {
        isWhiteHeartAnim_ = true;
        remainHeartFlashTimes_ = 6;  //3 times white-black change
    }
}
void PlayerStatus::setFP(float fp) {
    // mirror setHP behavior for foods so foods and hearts share same logic/animation
    float clampedFp = std::min(fp, maxFP_);
    int previousHalfFoods = static_cast<int>(std::round(2.f * fp_ / FP_PER_FOOD));
    int currentHalfFoods = static_cast<int>(std::round(2.f * clampedFp / FP_PER_FOOD));
    if (previousHalfFoods == currentHalfFoods) return;
    fp_ = fp;
    GameData::playerFP[GameData::playerType] = fp_;
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
                    foodStates_[i] = 1;
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
void PlayerStatus::render(sf::RenderWindow& window) {
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
        float shakeOffset = HEART_SIZE * 0.3f * std::pow((1 - std::min(hp_, maxHP_) / maxHP_), 9.f);  //shake when hp low
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
        float shakeOffset = FOOD_SIZE * 0.3f * std::pow((1 - std::min(fp_, maxFP_) / maxFP_), 9.f);  //shake when hp low
        foodSprite.move(Random::fastRandFloat(0, shakeOffset),
            Random::fastRandFloat(0, shakeOffset));
        if (isFoodJumping_ && static_cast<int>(i) == jumpingFoodIndex_) {
            foodSprite.move(0.f, -FOOD_SIZE * 0.25f);
        }
        window.draw(foodSprite);
    }
    window.setView(previousView);
}