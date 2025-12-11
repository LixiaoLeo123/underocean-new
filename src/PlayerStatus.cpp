#include "client/scenes/levelscenes/PlayerStatus.h"

#include "client/common/ResourceManager.h"

PlayerStatus::PlayerStatus() {
    uiView.setSize(UI_VIEW_WIDTH, 0.f);
    hpSprites[0].setTexture(ResourceManager::getTexture("assets/images/icons/heart00.png"));
    hpSprites[1].setTexture(ResourceManager::getTexture("assets/images/icons/heart10.png"));
    hpSprites[2].setTexture(ResourceManager::getTexture("assets/images/icons/heart11.png"));
    hpSprites[3].setTexture(ResourceManager::getTexture("assets/images/icons/heart20.png"));
    hpSprites[4].setTexture(ResourceManager::getTexture("assets/images/icons/heart21.png"));
    hpSprites[5].setTexture(ResourceManager::getTexture("assets/images/icons/heart22.png"));
    fpSprites[0].setTexture(ResourceManager::getTexture("assets/images/icons/food00.png"));
    fpSprites[1].setTexture(ResourceManager::getTexture("assets/images/icons/food10.png"));
    fpSprites[2].setTexture(ResourceManager::getTexture("assets/images/icons/food11.png"));
    fpSprites[3].setTexture(ResourceManager::getTexture("assets/images/icons/food20.png"));
    fpSprites[4].setTexture(ResourceManager::getTexture("assets/images/icons/food21.png"));
    fpSprites[5].setTexture(ResourceManager::getTexture("assets/images/icons/food22.png"));
    float heartOriginalSize = static_cast<float>(hpSprites[0].getTexture()->getSize().x);
    float heartScale = HEART_SIZE / heartOriginalSize;
    float foodOriginalSize = static_cast<float>(fpSprites[0].getTexture()->getSize().x);
    float foodScale = FOOD_SIZE / foodOriginalSize;
    for (int i = 0; i < 6; ++i) {
        hpSprites[i].setScale(heartScale, heartScale);
        fpSprites[i].setScale(foodScale, foodScale);
    }
}
