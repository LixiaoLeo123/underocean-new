#include "client/scenes/CharacterSelectMenu/CharacterSelectMenu.h"
#include "client/common/ResourceManager.h"
#include "client/scenes/startmenu/StartMenu.h"
#include "server/core(deprecate)/GameData.h"
#include <algorithm>
#include <cmath>
CharacterSlot::CharacterSlot(EntityTypeID t, bool isUnlocked, float yPos, float size)
    : type(t), unlocked(isUnlocked), originalY(yPos){
    anim.init(type);
    sf::FloatRect b = anim.sprite.getLocalBounds();
    float scale = size / std::max(b.width, b.height);
    anim.sprite.setScale(scale, scale);
    bgBox.setSize({size, size});
    bgBox.setFillColor(sf::Color(255,255,255,8));
    bgBox.setOutlineThickness(1);
    selectionFrame.setSize({size + 4, size + 4});
    selectionFrame.setFillColor(sf::Color::Transparent);
    selectionFrame.setOutlineThickness(2);
    selectionFrame.setOutlineColor(sf::Color::Yellow);
}
void CharacterSlot::render(sf::RenderWindow& window, bool isSelected, float currentY) {
    float x = CharacterSelectMenu::LIST_X;
    bgBox.setPosition(x, currentY);
    selectionFrame.setPosition(x - 2, currentY - 2);
    anim.sprite.setPosition(
        x + bgBox.getSize().x / 2.f,
        currentY + bgBox.getSize().y / 2.f
    );
    if (!unlocked) {
        anim.sprite.setColor(sf::Color::Black);
        bgBox.setOutlineColor(sf::Color(50,50,50));
    } else if (isHovered) {
        anim.sprite.setColor(sf::Color::White);
        bgBox.setOutlineColor(sf::Color::White);
    } else if (isSelected) {
        anim.sprite.setColor(sf::Color::White);
        bgBox.setOutlineColor(sf::Color::Yellow);
    } else {
        anim.sprite.setColor(sf::Color(180,180,180));
        bgBox.setOutlineColor(sf::Color(100,100,100));
    }
    window.draw(bgBox);
    if (isSelected) window.draw(selectionFrame);
    window.draw(anim.sprite);
}
bool CharacterSlot::contains(sf::Vector2f mousePos) const {
    return bgBox.getGlobalBounds().contains(mousePos);
}
CharacterSelectMenu::CharacterSelectMenu(const std::shared_ptr<SmoothTextLabel>& title)
    : title_(title),
      bgObj0_(ResourceManager::getTexture("images/backgrounds/bg1/bubble0.png"), HEIGHT, -0.1f, 0.f, 10.f, 0.06f),
      bgObj1_(ResourceManager::getTexture("images/backgrounds/bg1/bubble1.png"), HEIGHT, 0.15f, 1.f, 10.f, 0.06f),
      skillTexture_(ResourceManager::getTexture("images/icons/skills.png")),
      fontEn_(ResourceManager::getFont("fonts/font4.ttf")){
    background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg1/bg1.png"));
    title_->setVisible(true);
    title_->setOutlineColor(sf::Color::Black, 1);
    int idx = 0;
    float currentY = LIST_TOP;
    selectedType_ = static_cast<EntityTypeID>(GameData::playerType);
    #define X(name) \
    { \
        EntityTypeID type = EntityTypeID::name; \
        bool unlocked = GameData::isCharacterUnlocked(static_cast<int>(type)); \
        slots_.emplace_back(type, unlocked, currentY, SLOT_SIZE); \
        currentY += SLOT_SIZE + 10.0f; \
        idx++; \
    }
    PLAYER_ENTITY_TYPES
    #undef X
    float totalContentHeight = currentY - LIST_TOP;
    float viewHeight = LIST_BOTTOM - LIST_TOP;
    maxScrollOffset_ = std::max(0.0f, totalContentHeight - viewHeight + 20.0f);
    float centerX = WIDTH * 0.35f;
    const std::string labels[] = {"HP", "FP", "SPD"};
    const sf::Color colors[] = {sf::Color(255, 60, 60), sf::Color(60, 60, 255), sf::Color(60, 255, 60)};
    for(int i=0; i<3; ++i) {
        float y = HEIGHT * 0.72f + i * 22.0f;
        statLabels_[i].setFont(fontEn_);
        statLabels_[i].setString(labels[i]);
        statLabels_[i].setCharacterSize(64);
        statLabels_[i].setScale(0.25f, 0.25f);
        statLabels_[i].setPosition(centerX - 75, y - 5);
        statBarsBg_[i].setSize(sf::Vector2f(120, 10));
        statBarsBg_[i].setFillColor(sf::Color(40, 40, 40));
        statBarsBg_[i].setPosition(centerX - 40, y);
        statBars_[i].setSize(sf::Vector2f(0, 10));
        statBars_[i].setFillColor(colors[i]);
        statBars_[i].setPosition(centerX - 40, y);
    }
    float rightX = WIDTH * 0.64f;
    skillNameText_.setFont(fontEn_);
    skillNameText_.setCharacterSize(64);
    skillNameText_.setScale(0.25f, 0.25f);
    skillNameText_.setPosition(rightX - 60, HEIGHT * 0.68f);
    skillDescText_.setFont(fontEn_);
    skillDescText_.setCharacterSize(64);
    skillDescText_.setScale(0.25f, 0.25f);
    skillDescText_.setFillColor(sf::Color(200, 200, 200));
    skillDescText_.setPosition(rightX - 60, HEIGHT * 0.76f);
    skillDescText_.setString("Select your character\nPress Q to back");
    for(int i=0; i<4; ++i) {
        float sx = rightX + (i % 2) * 45;
        float sy = HEIGHT * 0.38f + (i / 2) * 45;
        skillSlots_[i].icon.setTexture(skillTexture_);
        skillSlots_[i].icon.setPosition(sx, sy);
        skillSlots_[i].icon.setScale(1.2f, 1.2f); // 32x32 -> ~38x38
        skillSlots_[i].bounds = sf::FloatRect(sx, sy, 38, 38);
        skillSlots_[i].levelText.setFont(fontEn_);
        skillSlots_[i].levelText.setCharacterSize(64);
        skillSlots_[i].levelText.setScale(0.156f, 0.156f);
        skillSlots_[i].levelText.setPosition(sx + 22, sy + 28);
    }
    selectCharacter(selectedType_);
    applyParamData();
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
    GameData::playerType = static_cast<int>(type);
    GameData::saveSettings();
    previewSprite_.init(type);
    previewSprite_.sprite.setScale(2.5f, 2.5f);
    previewSprite_.sprite.setPosition(WIDTH * 0.39f, HEIGHT * 0.45f);
    applyParamData();
    // switch(type) {
    //     #define X(name) case EntityTypeID::name: extractParamData<EntityTypeID::name>(); break;
    //     PLAYER_ENTITY_TYPES
    //     #undef X
    //     default: break;
    // }
}
void CharacterSelectMenu::applyParamData() {
    switch (selectedType_) {
#define X(type) case EntityTypeID::type: extractParamData<EntityTypeID::type>(); break;
        PLAYER_ENTITY_TYPES
#undef X
        default: break;
    }
}
template <EntityTypeID ID>
void CharacterSelectMenu::extractParamData() {
    using P = ParamTable<ID>;
    float hpPct = std::min(P::HP_BASE * 100.f / 1000.f, 1.f);
    float fpPct = std::min(P::FP_BASE * 100.f / 1000.f, 1.f);
    float spdPct = std::min(P::MAX_VELOCITY / 15.f, 1.f);
    statBars_[0].setSize(sf::Vector2f(120.f * hpPct, 10));
    statBars_[1].setSize(sf::Vector2f(120.f * fpPct, 10));
    statBars_[2].setSize(sf::Vector2f(120.f * spdPct, 10));
    for(int i=0; i<4; ++i) {
        int sIdx = P::SKILL_INDICES.skillIndices[i];
        int lvl = GameData::skillLevel[static_cast<int>(ID) * 4 + i];
        skillSlots_[i].skillIndex = sIdx;
        skillSlots_[i].skillLevel = lvl;
        int tx = (sIdx % 8) * 32;
        int ty = (sIdx / 8) * 32;
        skillSlots_[i].icon.setTextureRect(sf::IntRect(tx, ty, 32, 32));
        if(lvl == 0) {
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
    else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
        SceneSwitchRequest request = {
            SceneSwitchRequest::Replace,
            std::make_unique<StartMenu>(title_, true),
            0,
            2
        };
        onRequestSwitch_(request);
        return;
    }
    else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            scrollOffset_ -= event.mouseWheelScroll.delta * 20.0f;
            scrollOffset_ = std::max(0.0f, std::min(scrollOffset_, maxScrollOffset_));
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        for (const auto& slot : slots_) {
            float visualY = slot.originalY - scrollOffset_;
            if (visualY > LIST_TOP - SLOT_SIZE && visualY < LIST_BOTTOM) {
                if (slot.contains({static_cast<float>(event.mouseButton.x),
                    static_cast<float>(event.mouseButton.y)})) {
                    if (slot.unlocked) {
                        selectCharacter(slot.type);
                    } else {
                        //do something
                    }
                    break;
                }
            }
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        for (auto& slot : slots_) {
            float visualY = slot.originalY - scrollOffset_;
            if (visualY + SLOT_SIZE > LIST_TOP && visualY < LIST_BOTTOM) {
                slot.isHovered = slot.contains({static_cast<float>(event.mouseButton.x),
                    static_cast<float>(event.mouseButton.y)});
            }
        }
    }
}
void CharacterSelectMenu::update(float dt) {
    bgObj0_.update(dt);
    bgObj1_.update(dt);
    title_->updateTotal(dt);
    for (auto& slot : slots_) {
        slot.anim.update(dt);
    }
    previewSprite_.update(dt);
}
void CharacterSelectMenu::render(sf::RenderWindow& window) {
    if (!viewInit_) {
        sf::Vector2u winSize(window.getSize());
        resetViewArea(winSize.x, winSize.y);
        sf::Vector2f titleSize = title_->calculateSizeByWidth(WIDTH * 0.6f);
        title_->convertTo(view_);
        title_->setBounds(sf::Vector2f((WIDTH - titleSize.x) / 2.f, HEIGHT / 128.f), titleSize);
        viewInit_ = true;
    }
    if (viewDirty_) {
        viewDirty_ = false;
        window.setView(view_);
    }
    window.draw(background_);
    bgObj0_.render(window);
    bgObj1_.render(window);
    for (auto& slot : slots_) {
        float visualY = slot.originalY - scrollOffset_;
        // if (visualY + SLOT_SIZE > LIST_TOP && visualY < LIST_BOTTOM) {
        //TODO: add precise border check
            slot.render(window, slot.type == selectedType_, visualY);
        // }
    }
    window.draw(previewSprite_.sprite);
    for(int i=0; i<3; ++i) {
        window.draw(statBarsBg_[i]);
        window.draw(statBars_[i]);
        window.draw(statLabels_[i]);
    }
    bool anySkillHover = false;
    for(const auto& ss : skillSlots_) {
        window.draw(ss.icon);
        window.draw(ss.levelText);
        if (ss.bounds.contains(InputManager::getInstance().mousePosWorld)) {
            skillNameText_.setString("Skill ID: " + std::to_string(ss.skillIndex));
            if (ss.skillLevel == 0) {
                skillDescText_.setString("???");
                skillDescText_.setFillColor(sf::Color(255, 100, 100));
            } else {
                skillDescText_.setString(getSkillIntroduction(ss.skillIndex));
                skillDescText_.setFillColor(sf::Color(200, 200, 200));
            }
            anySkillHover = true;
        }
    }
    if(!anySkillHover) {
        skillNameText_.setString("Skill ID: ???");
        skillDescText_.setString("...");
        skillDescText_.setFillColor(sf::Color(200, 200, 200));
    }
    window.draw(skillNameText_);
    window.draw(skillDescText_);
    title_->render(window);
}
void CharacterSelectMenu::handleSwitchRequest(SceneSwitchRequest& req) {
     if (req.action == SceneSwitchRequest::None && req.extra == 2) {
         title_->setOldView(view_);
     } else {
         INodeScene::handleSwitchRequest(req);
     }
}
