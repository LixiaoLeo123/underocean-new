#ifndef UNDEROCEAN_CHARACTERSELECTMENU_H
#define UNDEROCEAN_CHARACTERSELECTMENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <memory>
#include "client/common/INodeScene.h"
#include "client/ui/widgets/labels/SmoothTextLabel.h"
#include "../FloatingObj.h"
#include "common/Types.h"

// 专门用于此界面的角色槽位控件
class CharacterSlot {
public:
    sf::Sprite sprite;
    sf::RectangleShape selectionFrame; // 选中框
    sf::RectangleShape bgBox;        // 背景框(用于增加点击区域)
    EntityTypeID type;
    bool unlocked;
    float originalY; // 用于滚动计算
    bool isHovered = false;

    CharacterSlot(EntityTypeID t, bool isUnlocked, float yPos, float size);

    // 渲染函数：处理黑色剪影逻辑
    void render(sf::RenderWindow& window, bool isSelected, float currentY);
    // 检查点击
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
    // --- 核心引用 ---
    std::shared_ptr<SmoothTextLabel> title_;

    // --- 视图控制 ---
    sf::View view_;
    bool viewInit_ = false;
    bool viewDirty_ = false;
    sf::Vector2f lastMouseWorldPos_; // 缓存鼠标在 View 中的位置

    // --- 布局常量 ---
    constexpr static int WIDTH = 576;
    constexpr static int HEIGHT = 324;
    // 列表显示区域
    constexpr static float LIST_X = WIDTH * 0.1f;
    constexpr static float LIST_TOP = HEIGHT * 0.22f;
    constexpr static float LIST_BOTTOM = HEIGHT * 0.88f;
    constexpr static float SLOT_SIZE = WIDTH * 0.08f;

    // --- 背景 ---
    sf::Sprite background_;
    FloatingObj bgObj0_;
    FloatingObj bgObj1_;

    // --- 角色列表 ---
    std::vector<CharacterSlot> slots_;
    float scrollOffset_ = 0.0f;
    float maxScrollOffset_ = 0.0f;

    // --- 中间预览 ---
    EntityTypeID selectedType_;
    sf::Sprite previewSprite_;
    sf::Text nameText_;
    // 属性条
    sf::RectangleShape statBars_[3];
    sf::RectangleShape statBarsBg_[3];
    sf::Text statLabels_[3];

    // --- 右侧技能 ---
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

    // --- 资源 ---
    sf::Font& fontEn_;
    sf::Font& fontCn_;

    // --- 内部逻辑 ---
    void resetViewArea(unsigned winWidth, unsigned winHeight);
    void selectCharacter(EntityTypeID type);

    // 模板函数：提取 Types.h 中的 ParamTable 数据
    template <EntityTypeID ID>
    void extractParamData();
    friend CharacterSlot;
};

#endif //UNDEROCEAN_CHARACTERSELECTMENU_H
