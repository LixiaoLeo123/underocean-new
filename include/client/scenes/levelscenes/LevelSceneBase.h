//
// Created by 15201 on 12/6/2025.
//

#ifndef UNDEROCEAN_LEVELSCENEBASE_H
#define UNDEROCEAN_LEVELSCENEBASE_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "PlayerStatus.h"
#include "SkillBar.h"
#include "client/common/ChatBox.h"
#include "client/common/ClientFoodBall.h"
#include "client/common/DeathEffectSystem.h"
#include "client/common/InputManager.h"
#include "client/common/IScene.h"
#include "client/common/LightingSystem.h"
#include "client/common/NetworkEntity.h"
#include "client/common/PlayerEntity.h"
#include "common/net(depricate)/PacketReader.h"
#include "common/net(depricate)/PacketWriter.h"
#include "common/network/ClientNetworkDriver.h"
#include "server/new/component/Components.h"



class LevelSceneBase : public IScene{
public:
    explicit LevelSceneBase(const std::shared_ptr<ClientNetworkDriver>& driver,
        ClientCommonPlayerAttributes& playerAttributes, const std::shared_ptr<ChatBox>& chatBox);
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event &event) override;
    void resetViewSize(unsigned windowWidth, unsigned windowHeight, float scale = 1.f);
    void correctView();  //keep view inside map size
    void update(float dt) override;
    [[nodiscard]] unsigned getCurrentTick() const { return currentTick_; }
    virtual UVector getMapSize() = 0;
    virtual std::uint16_t ltonX(float x) = 0;  //local to net x
    virtual float ntolX(std::uint16_t x) = 0;
    virtual std::uint16_t ltonY(float y) = 0;  //local to net y
    virtual float ntolY(std::uint16_t y) = 0;
protected:
    struct DamagePopup {
        sf::Text text;
        float lifetime;
        float maxLifetime;
        sf::Color initialColor;
    };
    std::vector<DamagePopup> damagePopups_;
    struct PauseMenu {
        bool active = false;
        sf::RectangleShape background;  //black, half-transparency
        struct Button {
            sf::Sprite sprite;
            sf::Vector2f baseScale;
            float currentScaleRatio = 1.0f;  //anim
            sf::FloatRect bounds;
            std::function<void()> onClick;
        };
        Button btnBack;
        Button btnRetry;
        Button btnResume;
        const float BUTTON_SPACING = 5.0f;
        const float BUTTON_Y_OFFSET = 0.0f;
    } pauseMenu_;
    void initPauseMenu();
    void updatePauseMenu(float dt, const sf::Vector2f& mouseWorldPos);
    void renderPauseMenu(sf::RenderWindow& window) const;
    void handlePauseInput(const sf::Event& event, const sf::Vector2f& mouseWorldPos);
    struct DialogueData {
        std::string text;
        std::string soundName;
    };
    struct DialogueSystem {
        bool active = false;
        bool canCloseOnEnter = false;
        int currentDialogueIndex = 0;
        std::vector<DialogueData> currentLines;
        size_t currentLineIndex = 0;
        std::string displayString;
        std::string fullTargetString;
        size_t charIndex = 0;
        float typeTimer = 0.f;
        const float TYPE_INTERVAL = 0.05f;
        sf::RectangleShape bgBox; //ui
        sf::Text textDrawable;
        std::map<int, std::vector<DialogueData>> localDatabase;
    } dialogueSystem_;
    void initDialogueSystem();
    void updateDialogueSystem(float dt);
    void renderDialogueSystem(sf::RenderWindow& window);
    void startDialogue(int index, bool canClose);
    void advanceDialogue();
    static std::string parseDialogueText(const std::string& raw);  //parse %xxx%
    DeathEffectSystem deathSystem_;
    sf::Sprite background_;
    sf::View view_ {};
    bool viewInit_ { false };  //not init
    std::shared_ptr<ChatBox> chatBox_;
    ClientCommonPlayerAttributes& playerAttributes_;
    void spawnDamagePopup(float delta, const sf::Vector2f& pos);
    void updateDamagePopups(float dt);
    void handleEntityStaticData();
    void handleEntityLeave();
    void handleEntityDynamic();
    void handleEntityDeath();
    void handleEntityHPChange();
    void handlePlayerStateUpdate();  //fp, hp
    void handleEntitySizeChange();
    void handlePlayerAttributesUpdate();  //max hp, max fp, max vel, max acc
    void handleMessagePacket();
    void handlePlayerDash();
    void handleSkillPackets();  //including skill ready, skill end and skill apply
    void handlePlayerRespawn();
    void handleHPDelta();
    void handleDialoguePacket();
    void handleGlowSetPacket();
    virtual int getLevelNum() = 0;
    enum class State {
        GAMING,
        DEATH
    } state_ = State::GAMING;
    constexpr static float DEATH_TEXT_DISPLAY_START = 1.2f;
    constexpr static float DEATH_TEXT_DISPLAY_END =5.5f;
    float deathTimer_ = 0.f;  //for anim
private:
    constexpr static float VIEW_WIDTH = 80.f;
    constexpr static float VIEW_HEIGHT = 45.f;
    float accDisRatio_ { -1.f };
    PlayerStatus playerStatus_ {};  //HP and FP indicator
    float playerGlowRadius_{0.f};
    sf::Color playerGlowColor_;
    float lastTime_ = 0.f;
    bool bgmInit_ { false };
protected:
    std::unordered_map<Entity, std::unique_ptr<NetworkEntity>> entities_;
    PlayerEntity player;
    std::shared_ptr<ClientNetworkDriver> driver_;  //given by LevelSelectMenu
    PacketWriter writer_;  //reuse
    SkillBar skillBar_;
    sf::Shader& deathFilter_;
    sf::Shader& blur_;
    unsigned currentTick_ { 0 };
    LightingSystem lightingSystem_;
    sf::Color ambientLightColorBase_ { 168, 168, 188 };
    sf::Color ambientLightColor_ { 168, 168, 188 };  //blue
    std::vector<ClientFoodBall> foodBalls_;
    void setTime(float time) {
        float phase = (time / MAX_TIME) * 2.0f * 3.14159f;
        float brightness = 0.5f * (1.0f + std::sinf(phase));
        ambientLightColor_.r = static_cast<sf::Uint8>(
            std::clamp(ambientLightColorBase_.r * brightness, 0.0f, 255.0f)
        );
        ambientLightColor_.g = static_cast<sf::Uint8>(
            std::clamp(ambientLightColorBase_.g * brightness, 0.0f, 255.0f)
        );
        ambientLightColor_.b = static_cast<sf::Uint8>(
            std::clamp(ambientLightColorBase_.b * brightness, 0.0f, 255.0f)
        );
    }
};
inline void LevelSceneBase::resetViewSize(unsigned windowWidth, unsigned windowHeight, float scale) {
    float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    constexpr static float viewRatio = VIEW_WIDTH / VIEW_HEIGHT;
    if (windowRatio > viewRatio) {
        float newHeight = VIEW_WIDTH / windowRatio;
        view_.setSize(VIEW_WIDTH * scale, newHeight * scale);
    } else {
        float newWidth = VIEW_HEIGHT * windowRatio;
        view_.setSize(newWidth * scale, VIEW_HEIGHT * scale);
    }
}
inline void LevelSceneBase::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
        if (!chatBox_->isOpen() && !dialogueSystem_.active && state_ != State::DEATH) {
            if (pauseMenu_.active) {
                pauseMenu_.active = false;
                driver_->send(nullptr, 0, ServerTypes::PKT_REQUEST_RESUME, true);
            }
            else {
                pauseMenu_.active = true;
                driver_->send(nullptr, 0, ServerTypes::PKT_REQUEST_STOP, true);
            }
        }
    }
    // if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
    //     startDialogue(1, true);
    // }
    handlePauseInput(event, InputManager::getInstance().mousePosWorld);
    if (dialogueSystem_.active) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            advanceDialogue();
        }
        //return;
    }
    // if event is window resize, adjust sf::View, to maintain its ratio same as the window
    // while keeping inside VIEW_WIDTH and VIEW_HEIGHT with max size
    if (event.type == sf::Event::Resized) {
        resetViewSize(event.size.width, event.size.height);
        correctView();
        playerStatus_.onWindowSizeChange(event.size.width, event.size.height);
        lightingSystem_.onWindowResize(event.size.width, event.size.height);
    }
    if (!pauseMenu_.active) {
        chatBox_->handleEvent(event);
        if (!chatBox_->isOpen())
            skillBar_.handleEvent(event);
    }
}
inline void LevelSceneBase::correctView() {
    if (view_.getCenter().x < view_.getSize().x / 2)
        view_.setCenter(view_.getSize().x / 2, view_.getCenter().y);
    else if (view_.getCenter().x > getMapSize().x - view_.getSize().x / 2)
        view_.setCenter(getMapSize().x - view_.getSize().x / 2, view_.getCenter().y);
    if (view_.getCenter().y < view_.getSize().y / 2)
        view_.setCenter(view_.getCenter().x, view_.getSize().y / 2);
    else if (view_.getCenter().y > getMapSize().y - view_.getSize().y / 2)
        view_.setCenter(view_.getCenter().x, getMapSize().y - view_.getSize().y / 2);
}
#endif //UNDEROCEAN_LEVELSCENEBASE_H
