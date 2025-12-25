#include "client/scenes/CharacterSelectMenu/CharacterSelectMenu.h"
#include "client/common/ResourceManager.h"
#include "client/scenes/startmenu/StartMenu.h"
#include "server/core(deprecate)/GameData.h"
#include <algorithm>
#include <cmath>

// ================= CharacterSlot 实现 =================

CharacterSlot::CharacterSlot(EntityTypeID t, bool isUnlocked, float yPos, float size)
    : type(t), unlocked(isUnlocked), originalY(yPos)
{
    // 获取贴图
    const sf::Texture& tex = ResourceManager::getTexture(getTexturePath(type));
    sprite.setTexture(tex);

    // 缩放适应大小
    sf::Vector2u texSize = tex.getSize();
    float scaleX = size / texSize.x;
    float scaleY = size / texSize.y;
    float scale = std::min(scaleX, scaleY); // 保持比例
    sprite.setScale(scale, scale);

    // 设置包围盒背景（用于点击判定）
    bgBox.setSize(sf::Vector2f(size, size));
    bgBox.setFillColor(sf::Color(255, 255, 255, 10)); // 极淡的背景
    bgBox.setOutlineThickness(1);
    bgBox.setOutlineColor(sf::Color(100, 100, 100, 50));

    // 设置选中框
    selectionFrame.setSize(sf::Vector2f(size + 4, size + 4));
    selectionFrame.setFillColor(sf::Color::Transparent);
    selectionFrame.setOutlineThickness(2);
    selectionFrame.setOutlineColor(sf::Color::Yellow);
}

void CharacterSlot::render(sf::RenderWindow& window, bool isSelected, float currentY) {
    float x = CharacterSelectMenu::LIST_X;

    // 更新位置
    sprite.setPosition(x + (bgBox.getSize().x - sprite.getGlobalBounds().width)/2,
                       currentY + (bgBox.getSize().y - sprite.getGlobalBounds().height)/2);
    bgBox.setPosition(x, currentY);
    selectionFrame.setPosition(x - 2, currentY - 2);

    // 核心：颜色逻辑
    if (!unlocked) {
        // 未解锁：纯黑剪影
        sprite.setColor(sf::Color::Black);
        bgBox.setOutlineColor(sf::Color(50, 50, 50));
    } else {
        if (isHovered) {
             sprite.setColor(sf::Color(255, 255, 255, 255)); // 亮
             bgBox.setOutlineColor(sf::Color::White);
        } else if (isSelected) {
             sprite.setColor(sf::Color(255, 255, 255, 255));
             bgBox.setOutlineColor(sf::Color::Yellow);
        } else {
             sprite.setColor(sf::Color(180, 180, 180, 255)); // 稍微暗一点
             bgBox.setOutlineColor(sf::Color(100, 100, 100));
        }
    }

    // 绘制
    window.draw(bgBox);
    if (isSelected) window.draw(selectionFrame);
    window.draw(sprite);
}

bool CharacterSlot::contains(sf::Vector2f mousePos) const {
    return bgBox.getGlobalBounds().contains(mousePos);
}

// ================= Menu 实现 =================

CharacterSelectMenu::CharacterSelectMenu(const std::shared_ptr<SmoothTextLabel>& title)
    : title_(title),
      fontEn_(ResourceManager::getFont("fonts/font0.otf")),
      fontCn_(ResourceManager::getFont("fonts/font4.ttf")),
      skillTexture_(ResourceManager::getTexture("images/icons/skills.png")),
      bgObj0_(ResourceManager::getTexture("images/backgrounds/bg1/bubble0.png"), HEIGHT, -0.1f, 0.f, 10.f, 0.06f),
      bgObj1_(ResourceManager::getTexture("images/backgrounds/bg1/bubble1.png"), HEIGHT, 0.15f, 1.f, 10.f, 0.06f)
{
    background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg1/bg1.png"));

    title_->setVisible(true);
    title_->setOutlineColor(sf::Color::Black, 1);

    // 1. 初始化角色列表
    // 使用 X-Macro 遍历所有 Types.h 中定义的 PLAYER 角色
    int idx = 0;
    float currentY = LIST_TOP;

    // 初始化选中的类型 (从GameData读取)
    selectedType_ = static_cast<EntityTypeID>(GameData::playerType);

    #define X(name) \
    { \
        EntityTypeID type = EntityTypeID::name; \
        bool unlocked = GameData::isCharacterUnlocked(static_cast<int>(type)); \
        slots_.emplace_back(type, unlocked, currentY, SLOT_SIZE); \
        currentY += SLOT_SIZE + 10.0f; /* 间距 */ \
        idx++; \
    }
    PLAYER_ENTITY_TYPES
    #undef X

    // 计算滚动极限
    float totalContentHeight = currentY - LIST_TOP;
    float viewHeight = LIST_BOTTOM - LIST_TOP;
    maxScrollOffset_ = std::max(0.0f, totalContentHeight - viewHeight + 20.0f);

    // 2. 属性条初始化 (中间)
    float centerX = WIDTH * 0.45f;
    const std::string labels[] = {"HP", "FP", "SPD"};
    const sf::Color colors[] = {sf::Color(255, 60, 60), sf::Color(60, 60, 255), sf::Color(60, 255, 60)};

    for(int i=0; i<3; ++i) {
        float y = HEIGHT * 0.65f + i * 22.0f;

        statLabels_[i].setFont(fontEn_);
        statLabels_[i].setString(labels[i]);
        statLabels_[i].setCharacterSize(12);
        statLabels_[i].setPosition(centerX - 70, y - 2);

        statBarsBg_[i].setSize(sf::Vector2f(120, 10));
        statBarsBg_[i].setFillColor(sf::Color(40, 40, 40));
        statBarsBg_[i].setPosition(centerX - 40, y);

        statBars_[i].setSize(sf::Vector2f(0, 10));
        statBars_[i].setFillColor(colors[i]);
        statBars_[i].setPosition(centerX - 40, y);
    }

    nameText_.setFont(fontEn_);
    nameText_.setCharacterSize(24);
    nameText_.setFillColor(sf::Color::White);
    nameText_.setOutlineColor(sf::Color::Black);
    nameText_.setOutlineThickness(1);

    // 3. 技能初始化 (右侧)
    float rightX = WIDTH * 0.78f;

    skillNameText_.setFont(fontCn_);
    skillNameText_.setCharacterSize(16);
    skillNameText_.setPosition(rightX - 50, HEIGHT * 0.65f);

    skillDescText_.setFont(fontCn_);
    skillDescText_.setCharacterSize(12);
    skillDescText_.setFillColor(sf::Color(200, 200, 200));
    skillDescText_.setPosition(rightX - 50, HEIGHT * 0.72f);
    skillDescText_.setString(L"选择角色查看技能\nQ键返回主菜单");

    for(int i=0; i<4; ++i) {
        float sx = rightX + (i % 2) * 42;
        float sy = HEIGHT * 0.35f + (i / 2) * 42;

        skillSlots_[i].icon.setTexture(skillTexture_);
        skillSlots_[i].icon.setPosition(sx, sy);
        skillSlots_[i].icon.setScale(1.2f, 1.2f); // 32x32 -> ~38x38

        skillSlots_[i].bounds = sf::FloatRect(sx, sy, 38, 38);

        skillSlots_[i].levelText.setFont(fontEn_);
        skillSlots_[i].levelText.setCharacterSize(10);
        skillSlots_[i].levelText.setPosition(sx + 24, sy + 24);
        skillSlots_[i].levelText.setOutlineColor(sf::Color::Black);
        skillSlots_[i].levelText.setOutlineThickness(1);
    }

    // 初始加载当前角色数据
    selectCharacter(selectedType_);
}

void CharacterSelectMenu::resetViewArea(unsigned winWidth, unsigned winHeight) {
    float windowRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);
    sf::Vector2f viewSize;
    if (windowRatio < static_cast<float>(WIDTH) / HEIGHT) {
        viewSize = sf::Vector2f(HEIGHT * windowRatio, HEIGHT);
    } else {
        viewSize = sf::Vector2f(WIDTH, WIDTH / windowRatio);
    }
    view_.reset(sf::FloatRect(0, 0, viewSize.x, viewSize.y));
    view_.setCenter(WIDTH / 2.f, HEIGHT / 2.f);
    viewDirty_ = true;
}

void CharacterSelectMenu::selectCharacter(EntityTypeID type) {
    selectedType_ = type;

    // 立即保存到 GameData
    GameData::playerType = static_cast<int>(type);
    GameData::saveSettings();

    // 更新预览 Sprite
    previewSprite_.setTexture(ResourceManager::getTexture(getTexturePath(type)));
    sf::FloatRect b = previewSprite_.getLocalBounds();
    previewSprite_.setOrigin(b.width/2, b.height/2);

    // 自动缩放预览图
    float scale = 1.0f;
    if(b.width < 64) scale = 2.5f;
    else if(b.width < 128) scale = 1.5f;
    previewSprite_.setScale(scale, scale);
    previewSprite_.setPosition(WIDTH * 0.45f, HEIGHT * 0.45f);

    // 调用模板函数加载数据
    switch(type) {
        #define X(name) case EntityTypeID::name: extractParamData<EntityTypeID::name>(); break;
        PLAYER_ENTITY_TYPES
        #undef X
        default: break;
    }
}

template <EntityTypeID ID>
void CharacterSelectMenu::extractParamData() {
    using P = ParamTable<ID>;

    // 1. 设置名称 (如果有映射表更好，这里简单处理)
    std::string name = "Entity " + std::to_string(static_cast<int>(ID));
    if (ID == EntityTypeID::SMALL_YELLOW) name = "Small Yellow";
    else if (ID == EntityTypeID::ROUND_GREEN) name = "Round Green";
    else if (ID == EntityTypeID::BALL_ORANGE) name = "Orange Ball";
    else if (ID == EntityTypeID::FLY_FISH) name = "Fly Fish";
    else if (ID == EntityTypeID::BLUE_LONG) name = "Blue Long";

    nameText_.setString(name);
    sf::FloatRect b = nameText_.getLocalBounds();
    nameText_.setOrigin(b.width/2, 0);
    nameText_.setPosition(WIDTH * 0.45f, HEIGHT * 0.35f);

    // 2. 设置属性条 (HP/FP/SPD)
    // 假设基准值：MaxHP=2000, MaxFP=2000, MaxSpd=20
    float hpPct = std::min(P::HP_BASE * 100.f / 1000.f, 1.f);
    float fpPct = std::min(P::FP_BASE * 100.f / 1000.f, 1.f);
    float spdPct = std::min(P::MAX_VELOCITY / 15.f, 1.f);

    statBars_[0].setSize(sf::Vector2f(120.f * hpPct, 10));
    statBars_[1].setSize(sf::Vector2f(120.f * fpPct, 10));
    statBars_[2].setSize(sf::Vector2f(120.f * spdPct, 10));

    // 3. 设置技能
    for(int i=0; i<4; ++i) {
        int sIdx = P::SKILL_INDICES.skillIndices[i];
        // 动态读取 GameData 中的技能等级
        int lvl = GameData::skillLevel[static_cast<int>(ID) * 4 + i];

        skillSlots_[i].skillIndex = sIdx;
        skillSlots_[i].skillLevel = lvl;

        // TextureRect 计算 (8列)
        int tx = (sIdx % 8) * 32;
        int ty = (sIdx / 8) * 32;
        skillSlots_[i].icon.setTextureRect(sf::IntRect(tx, ty, 32, 32));

        if(lvl == 0) {
            // 未解锁技能：变黑
            skillSlots_[i].icon.setColor(sf::Color(50, 50, 50));
            skillSlots_[i].levelText.setString("LOCK");
            skillSlots_[i].levelText.setFillColor(sf::Color::Red);
        } else {
            skillSlots_[i].icon.setColor(sf::Color::White);
            skillSlots_[i].levelText.setString("Lv." + std::to_string(lvl));
            skillSlots_[i].levelText.setFillColor(sf::Color::Yellow);
        }
    }
}

void CharacterSelectMenu::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        resetViewArea(event.size.width, event.size.height);
    }

    // Q 键返回 StartMenu
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
        SceneSwitchRequest request = {
            SceneSwitchRequest::Replace,
            std::make_unique<StartMenu>(title_, true), // true保持标题动画位置
            0,
            2 // 特殊标记，通知 StartMenu 重置视图
        };
        onRequestSwitch_(request);
        return;
    }

    // 滚轮控制列表
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            scrollOffset_ -= event.mouseWheelScroll.delta * 20.0f;
            // 限制滚动范围
            scrollOffset_ = std::max(0.0f, std::min(scrollOffset_, maxScrollOffset_));
        }
    }

    // 处理点击 (利用 render 更新后的 lastMouseWorldPos_)
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        for (const auto& slot : slots_) {
            float visualY = slot.originalY - scrollOffset_;
            // 简单的裁剪判断：只响应显示区域内的点击
            if (visualY > LIST_TOP - SLOT_SIZE && visualY < LIST_BOTTOM) {
                if (slot.contains(lastMouseWorldPos_)) {
                    if (slot.unlocked) {
                        selectCharacter(slot.type);
                    } else {
                        // 播放 "Locked" 音效?
                    }
                    break;
                }
            }
        }
    }
}

void CharacterSelectMenu::update(float dt) {
    static float time = 0.f;
    time += dt;

    bgObj0_.update(dt);
    bgObj1_.update(dt);
    title_->updateTotal(dt);

    // 预览图上下浮动
    float floatOffset = std::sin(time * 3.f) * 4.f;
    previewSprite_.setPosition(WIDTH * 0.45f, HEIGHT * 0.45f + floatOffset);
}

void CharacterSelectMenu::render(sf::RenderWindow& window) {
    // 1. 视图初始化
    if (!viewInit_) {
        sf::Vector2u winSize(window.getSize());
        resetViewArea(winSize.x, winSize.y);

        // 标题平滑移动到顶部
        sf::Vector2f titleSize = title_->calculateSizeByWidth(WIDTH * 0.6f);
        title_->convertTo(view_);
        title_->setBounds(sf::Vector2f((WIDTH - titleSize.x) / 2.f, 5.0f), titleSize);

        viewInit_ = true;
    }
    if (viewDirty_) {
        viewDirty_ = false;
        window.setView(view_);
    }

    // 2. 关键：更新鼠标世界坐标 (解决 getMousePosition 错误)
    // 注意：SFML 鼠标位置是相对窗口的，需要 mapPixelToCoords 转为 View 坐标
    lastMouseWorldPos_ = window.mapPixelToCoords(sf::Mouse::getPosition(window), view_);

    // 3. 绘制背景
    window.draw(background_);
    bgObj0_.render(window);
    bgObj1_.render(window);

    // 4. 绘制角色列表
    // 简单的裁剪逻辑：只绘制在显示范围内的 Slot
    for (auto& slot : slots_) {
        float visualY = slot.originalY - scrollOffset_;

        if (visualY + SLOT_SIZE > LIST_TOP && visualY < LIST_BOTTOM) {
            // 更新 Hover 状态
            slot.isHovered = slot.contains(lastMouseWorldPos_);
            // 渲染 (会自动处理黑影)
            slot.render(window, slot.type == selectedType_, visualY);
        }
    }

    // 5. 绘制中间预览
    window.draw(previewSprite_);
    window.draw(nameText_);

    for(int i=0; i<3; ++i) {
        window.draw(statBarsBg_[i]);
        window.draw(statBars_[i]);
        window.draw(statLabels_[i]);
    }

    // 6. 绘制技能 & 悬停检测
    bool anySkillHover = false;
    for(const auto& ss : skillSlots_) {
        window.draw(ss.icon);
        window.draw(ss.levelText);

        if (ss.bounds.contains(lastMouseWorldPos_)) {
            skillNameText_.setString(L"技能 ID: " + std::to_wstring(ss.skillIndex));
            // 实际项目中应查 SkillData 表
            if (ss.skillLevel == 0) {
                skillDescText_.setString(L"该技能尚未解锁\n提升等级以获取");
                skillDescText_.setFillColor(sf::Color(255, 100, 100));
            } else {
                skillDescText_.setString(L"技能等级: " + std::to_wstring(ss.skillLevel) +
                                       L"\n强大的深海技能，\n消耗 FP 发动。");
                skillDescText_.setFillColor(sf::Color(200, 200, 200));
            }
            anySkillHover = true;
        }
    }

    if(!anySkillHover) {
        skillNameText_.setString(L"角色选择");
        skillDescText_.setString(L"滚轮翻页列表\n点击已解锁角色\nQ键返回");
        skillDescText_.setFillColor(sf::Color(200, 200, 200));
    }

    window.draw(skillNameText_);
    window.draw(skillDescText_);

    // 7. 标题
    title_->render(window);
}

void CharacterSelectMenu::handleSwitchRequest(SceneSwitchRequest& req) {
     if (req.action == SceneSwitchRequest::None && req.extra == 2) {
         title_->setOldView(view_);
     } else {
         INodeScene::handleSwitchRequest(req);
     }
}
