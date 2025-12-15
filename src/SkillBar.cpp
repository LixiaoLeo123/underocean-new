#include "client/scenes/levelscenes/SkillBar.h"

#include <SFML/Graphics/RenderWindow.hpp>
SkillBar::SkillBar()
    :greyShader_(ResourceManager::getShader("shaders/grayscale.frag")) {
    skills_.setTexture(ResourceManager::getTexture("images/icons/skills.png"));
    skills_.setOrigin(FRAME_WIDTH / 2.f, FRAME_HEIGHT / 2.f);
    skills_.setScale(SKILL_ACTIVE_SCALE, SKILL_ACTIVE_SCALE);
    border_.setTexture(ResourceManager::getTexture("images/icons/skillborder.png"));
    border_.setOrigin(BORDER_WIDTH / 2.f, BORDER_HEIGHT / 2.f);
    border_.setScale(SKILL_ACTIVE_SCALE, SKILL_ACTIVE_SCALE); //same scale
}
void SkillBar::render(sf::RenderWindow &window) {
    if (windowRatio_ < 0.f) {
        onWindowSizeChange(window.getSize().x, window.getSize().y);
        windowRatio_ = static_cast<float>(window.getSize().y) / static_cast<float>(window.getSize().x);
    }
    sf::View originalView{window.getView()};
    window.setView(uiView_);
    float startX = UI_VIEW_WIDTH - SKILL_WIDTH * ICON_BOX_SCALE * static_cast<float>(totalUnlockedSkills_);
    for (int i = 0; i < 4; ++i) {
        if (!isSkillUnlocked_[i]) continue;
        skills_.setScale(skillScales_[i], skillScales_[i]);
        skills_.setTextureRect(skillRects_[i]);
        skills_.setPosition(startX + SKILL_WIDTH * ICON_BOX_SCALE / 2.f,
                            SKILL_HEIGHT * ICON_BOX_SCALE / 2.f);
        if (isSkillColorful_[i])
            window.draw(skills_);
        else
            window.draw(skills_, &greyShader_);
        //draw border if active
        if (isSkillActive_[i]) {
            border_.setPosition(startX + SKILL_WIDTH * ICON_BOX_SCALE / 2.f,
                                SKILL_HEIGHT * ICON_BOX_SCALE / 2.f);
            window.draw(border_);
        }
        startX += SKILL_WIDTH * ICON_BOX_SCALE;
    }
    window.setView(originalView);
}
