#ifndef UNDEROCEAN_CHARACTERSELECTMENU_H
#define UNDEROCEAN_CHARACTERSELECTMENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <memory>
#include "client/common/INodeScene.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "../FloatingObj.h"
#include "client/common/ResourceManager.h"
#include "common/Types.h"
#include "common/utils/Random.h"

struct AnimatedSpriteUI {
    sf::Sprite sprite;
    EntityTypeID type;
    int totalFrames = 1;
    int currentFrame = 0;
    float frameInterval = 1.f;
    float elapsed = 0.f;
    int frameWidth = 0;
    int frameHeight = 0;
    void init(EntityTypeID t) {
        type = t;
        const sf::Texture& tex = ResourceManager::getTexture(getTexturePath(type));
        sprite.setTexture(tex);
        totalFrames = getTextureTotalFrame(type);
        frameInterval = getFrameInterval(type);
        frameWidth  = tex.getSize().x / totalFrames;
        frameHeight = tex.getSize().y;
        sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
        elapsed = Random::randFloat(0.f, 10.f);
        currentFrame = (currentFrame + 1) % totalFrames;
        sprite.setTextureRect(sf::IntRect(
            currentFrame * frameWidth,
            0,
            frameWidth,
            frameHeight
        ));
    }
    void update(float dt) {
        if (totalFrames <= 1) return;
        elapsed += dt;
        if (elapsed >= frameInterval) {
            elapsed -= frameInterval;
            currentFrame = (currentFrame + 1) % totalFrames;
            sprite.setTextureRect(sf::IntRect(
                currentFrame * frameWidth,
                0,
                frameWidth,
                frameHeight
            ));
        }
    }
};

class CharacterSlot {
public:
    AnimatedSpriteUI anim;
    sf::RectangleShape selectionFrame;
    sf::RectangleShape bgBox;
    EntityTypeID type;
    bool unlocked;
    float originalY;
    bool isHovered = false;
    CharacterSlot(EntityTypeID t, bool isUnlocked, float yPos, float size);
    void render(sf::RenderWindow& window, bool isSelected, float currentY);
    bool contains(sf::Vector2f mousePos) const;
};

class CharacterSelectMenu : public INodeScene {
public:
    explicit CharacterSelectMenu(const std::shared_ptr<SmoothTextLabel>& title);
    ~CharacterSelectMenu() override = default;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void handleSwitchRequest(SceneSwitchRequest &req) override;
private:
    std::shared_ptr<SmoothTextLabel> title_;
    sf::View view_;
    bool viewInit_ = false;
    bool viewDirty_ = false;
    constexpr static int WIDTH = 576;
    constexpr static int HEIGHT = 324;
    constexpr static float LIST_X = WIDTH * 0.05f;
    constexpr static float LIST_TOP = HEIGHT * 0.22f;
    constexpr static float LIST_BOTTOM = HEIGHT * 0.9f;
    constexpr static float SLOT_SIZE = WIDTH * 0.1f;
    sf::Sprite background_;
    FloatingObj bgObj0_;
    FloatingObj bgObj1_;
    std::vector<CharacterSlot> slots_;
    float scrollOffset_ = 0.0f;
    float maxScrollOffset_ = 0.0f;
    EntityTypeID selectedType_;  //middle preview
    AnimatedSpriteUI previewSprite_;
    sf::RectangleShape statBars_[3];  //stat
    sf::RectangleShape statBarsBg_[3];
    sf::Text statLabels_[3];
    struct SkillSlot {
        sf::Sprite icon;
        sf::Text levelText;
        int skillIndex = 0;
        int skillLevel = 0;
        sf::FloatRect bounds;
    };
    std::array<SkillSlot, 4> skillSlots_;
    sf::Texture& skillTexture_;
    sf::Text skillNameText_;
    sf::Text skillDescText_;
    sf::Font& fontEn_;
    void resetViewArea(unsigned winWidth, unsigned winHeight);
    void selectCharacter(EntityTypeID type);
    template <EntityTypeID ID>
    void extractParamData();
    void applyParamData();  //without template, add switch dispatch
    friend CharacterSlot;
};

#endif //UNDEROCEAN_CHARACTERSELECTMENU_H
