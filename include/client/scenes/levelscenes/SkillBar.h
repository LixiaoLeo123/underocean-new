//
// Created by 15201 on 12/15/2025.
//

#ifndef UNDEROCEAN_SKILLBAR_H
#define UNDEROCEAN_SKILLBAR_H
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include "client/common/ResourceManager.h"
#include <SFML/Window/Event.hpp>

#include "client/common/KeyBindingManager.h"
#include "common/net(depricate)/PacketWriter.h"
#include "common/network/ClientNetworkDriver.h"

class PacketWriter;

namespace sf {
    class RenderWindow;
}

class SkillBar {
public:
    explicit SkillBar(ClientNetworkDriver& driver);
    void setSkillUnlocked(int index, bool unlocked);
    void setSkills(const std::uint8_t* skillIndices);  //different entity types have different skill icons
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void setSkillActive(int index, bool active);
    void setSkillColorful(int index, bool isColorful);  //all skills colorful or gray, isColorful means ready
    void update();
private:
    static constexpr float UI_VIEW_WIDTH = 128.f;
    static constexpr int FRAME_WIDTH = 32;  //pixel, original
    static constexpr int FRAME_HEIGHT = 32;
    static constexpr int BORDER_WIDTH = 44;
    static constexpr int BORDER_HEIGHT = 44;
    static constexpr float ICON_BOX_SCALE = 1.35f;  //scale to make icons separate
    static constexpr float SKILL_WIDTH = 3.f;  //after scale
    static constexpr float SKILL_HEIGHT = 3.f;
    constexpr static float SKILL_ACTIVE_SCALE = SKILL_WIDTH / FRAME_WIDTH;
    constexpr static float SKILL_INACTIVE_SCALE = SKILL_ACTIVE_SCALE * BORDER_WIDTH / FRAME_WIDTH;
    ClientNetworkDriver& driver_; //to send packet
    PacketWriter writer_;  //reuse
    float windowRatio_{-1.f};  //height / width, -1.f for not init
    int skillIndices_[4] {0, 0, 0, 0};  //only for render
    int totalUnlockedSkills_{0};
    sf::View uiView_ {};
    sf::Sprite skills_;
    sf::Sprite border_;  //skill border, enabled when skill is running
    sf::Shader& greyShader_;
    bool isSkillActive_[4] {};
    bool isSkillInAnim_[4] {};
    bool isSkillUnlocked_[4] {};
    bool isSkillColorful_[4]{};
    float skillScales_[4] {SKILL_INACTIVE_SCALE, SKILL_INACTIVE_SCALE, SKILL_INACTIVE_SCALE, SKILL_INACTIVE_SCALE};  //for stimulate anim
    sf::IntRect skillRects_[4];
    bool isKeyPressedLastFrame_[4] {false, false, false, false};
    void sendTryCastSkillPacket(int index);  //notify server
    void onWindowSizeChange(unsigned newWidth, unsigned newHeight) {
        float windowRatio = static_cast<float>(newHeight) / static_cast<float>(newWidth);
        uiView_.setSize(UI_VIEW_WIDTH, UI_VIEW_WIDTH * windowRatio);
        uiView_.setCenter(uiView_.getSize() / 2.f);
    }
    void recalcTotalUnlockedSkills() {
        totalUnlockedSkills_ = 0;
        for (bool unlocked : isSkillUnlocked_) {
            if (unlocked) ++totalUnlockedSkills_;
        }
    }
};
inline void SkillBar::update() {
    constexpr float ALPHA = 0.05f;  //less is smoother
    for (int i = 0; i < 4; ++i) {
        if (isSkillInAnim_[i]) {
            float currentScale = skillScales_[i];
            float targetScale = isSkillActive_[i] ? SKILL_ACTIVE_SCALE : SKILL_INACTIVE_SCALE;
            skillScales_[i] = currentScale * (1.f - ALPHA) + targetScale * ALPHA;
            if (std::abs(skillScales_[i] - targetScale) < 0.001f) {
                skillScales_[i] = targetScale;
                isSkillInAnim_[i] = false;
            }
        }
    }
}
inline void SkillBar::setSkillActive(int index, bool active) {
    assert(!(index < 0 || index >=4));
    if (isSkillActive_[index] != active) {
        isSkillInAnim_[index] = true;  //start anim
        isSkillActive_[index] = active;
    }
}
inline void SkillBar::setSkillUnlocked(int index, bool unlocked) {
    assert(!(index < 0 || index >=4));
    isSkillUnlocked_[index] = unlocked;
    recalcTotalUnlockedSkills();
}
inline void SkillBar::setSkills(const std::uint8_t* skillIndices) {  //different entity types have different skill icons
    for (int i = 0; i < 4; ++i) {
        skillRects_[i] = sf::IntRect(FRAME_WIDTH * (skillIndices[i] % 8),
            FRAME_HEIGHT * (skillIndices[i] / 8),
            FRAME_WIDTH,
            FRAME_HEIGHT);
        skillIndices_[i] = skillIndices[i];
    }
}
inline void SkillBar::setSkillColorful(int index, bool isColorful) {
    assert(!(index < 0 || index >=4));
    isSkillColorful_[index] = isColorful;
}
#endif //UNDEROCEAN_SKILLBAR_H
