#include "client/scenes/levelscenes/LevelSceneBase.h"

#include "client/common/AudioManager.h"
#include "client/common/ClientFoodBall.h"

LevelSceneBase::LevelSceneBase(const std::shared_ptr<ClientNetworkDriver>& driver,
                               ClientCommonPlayerAttributes& playerAttributes, const std::shared_ptr<ChatBox>& chatBox)
    : driver_(driver), skillBar_(*driver), playerAttributes_(playerAttributes), chatBox_(chatBox),
    deathFilter_(ResourceManager::getShader("shaders/deathfilter.frag")),
    blur_(ResourceManager::getShader("shaders/blur.frag")) {
    player.setType(static_cast<EntityTypeID>(GameData::playerType));
    player.setSize(GameData::playerSize[GameData::playerType]);
    player.setMaxVec(playerAttributes.maxVec);
    player.setMaxAcc(playerAttributes.maxAcc);
    playerStatus_.setMaxHP(playerAttributes.maxHP);
    playerStatus_.setMaxFP(playerAttributes.maxFP);
    playerStatus_.setHP(GameData::playerHP[GameData::playerType]);
    playerStatus_.setFP(GameData::playerFP[GameData::playerType]);
    // get max Acc when mouse is at the left or right side of the view
    accDisRatio_ = player.getMaxAcceleration() * 5 / VIEW_WIDTH;
    skillBar_.setSkills(playerAttributes_.skillIndices);
    for (int i = 0; i < 4; ++i) {
        if (GameData::getSkillLevel(i)) {
            skillBar_.setSkillUnlocked(i, true);
        }
    }
    //DeathEffectSystem
    sf::Texture* noise = &ResourceManager::getTexture("images/others/noise.png");
    sf::Texture* particle = &ResourceManager::getTexture("images/others/particle_round.png");
    deathSystem_.init(noise, particle);
    //player death shader
    deathFilter_.setUniform("texture", sf::Shader::CurrentTexture);
    blur_.setUniform("texture", sf::Shader::CurrentTexture);
    initPauseMenu();
    initDialogueSystem();
    driver_->setOnDisconnect([this]() {
        chatBox_->addMessage("&c&lError: &r&cDisconnected from server. Try reconnecting... &r&b&l&ka");
        SceneSwitchRequest request = {
                    SceneSwitchRequest::Pop,
                    nullptr,
                    0,
                    0
                };
        onRequestSwitch_(request);
    });
}
void LevelSceneBase::handlePlayerStateUpdate() {
    while (auto packet = driver_->popPacket(PacketType::PKT_PLAYER_STATE_UPDATE)) {
        if (driver_->hasPacket(PKT_PLAYER_STATE_UPDATE)) continue;  //only handle latest packet
        if (packet->size() != 6) continue;  //wrong packet
        PacketReader reader(std::move(*packet));
        std::uint16_t netHP = reader.nextUInt16();
        std::uint16_t netFP = reader.nextUInt16();
        std::uint16_t netSize = reader.nextUInt16();
        float hp = ntolHP16(netHP);
        float fp = ntolFP(netFP);
        float size = ntolSize16(netSize);
        playerStatus_.setHP(hp);
        playerStatus_.setFP(fp);
        player.setSize(size);
    }
}
void LevelSceneBase::render(sf::RenderWindow &window) {
    if (!viewInit_) {
        resetViewSize(window.getSize().x, window.getSize().y);
        correctView();
        lightingSystem_.init(window.getSize().x, window.getSize().y);
        viewInit_ = true;
    }
    if (state_ == State::DEATH) {
        resetViewSize(window.getSize().x, window.getSize().y,
            0.5f * std::expf(-deathTimer_) + 0.5f);
    }
    window.setView(view_);  //change here
    if (state_ == State::DEATH) {
        sf::RenderTexture tempRT, finalRT;
        tempRT.create(window.getSize().x, window.getSize().y);
        finalRT.create(window.getSize().x, window.getSize().y);
        // tempRT.setView(view_);
        // finalRT.setView(view_);
        // blur_.setUniform("texelSize", sf::Vector2f(1.f / tempRT.getSize().x, 1.f / tempRT.getSize().y));
        // blur_.setUniform("radius", static_cast<int>(8.f * std::min(deathTimer_, DEATH_TEXT_DISPLAY_START)));
        // // blur_.setUniform("radius", 1);
        // blur_.setUniform("horizontal", 1);
        // tempRT.clear();
        // tempRT.draw(background_, &blur_);
        // tempRT.display();
        // sf::Sprite tempSprite(tempRT.getTexture());
        // blur_.setUniform("horizontal", 0);
        // blur_.setUniform("texelSize", sf::Vector2f(1.f / finalRT.getSize().x, 1.f / finalRT.getSize().y));
        // finalRT.clear();
        // finalRT.draw(tempSprite, &blur_);
        // finalRT.display();
        // tempSprite.setTexture(finalRT.getTexture());
        deathFilter_.setUniform("u_intensity", std::clamp(deathTimer_ / DEATH_TEXT_DISPLAY_END, 0.f, 1.f));
        deathFilter_.setUniform("u_time", deathTimer_);
        tempRT.clear();
        tempRT.draw(background_, &deathFilter_);
        tempRT.display();
        sf::Sprite resultSprite(tempRT.getTexture());
        window.clear();
        window.draw(resultSprite);
    }
    else {
        window.draw(background_);
    }
    for (auto& food : foodBalls_) {
        food.render(window);
    }
    for (auto& pair : entities_) {
        pair.second->render(window);
    }
    if (state_ != State::DEATH)
        player.render(window);
    deathSystem_.render(window);
    for (const auto& popup : damagePopups_) {
        window.draw(popup.text);
    }
    //light
    lightingSystem_.clear(ambientLightColor_);
    for (auto& pair : entities_) {
        if (pair.second->hasLight()) {
            lightingSystem_.drawLight(
                pair.second->getPosition(),
                view_,
                pair.second->getLightRadius(),
                pair.second->getLightColor()
            );
        }
    }
    lightingSystem_.display();
    sf::View currentView = window.getView();
    window.setView(sf::View(sf::FloatRect(0.f, 0.f,
    static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y))));
    sf::Sprite lightMapSprite = lightingSystem_.getLightMapSprite();
    window.draw(lightMapSprite, sf::BlendMultiply);
    window.setView(currentView);
    //render UI
    playerStatus_.render(window);
    chatBox_->render(window);
    skillBar_.render(window);
    if (state_ == State::DEATH && deathTimer_ > DEATH_TEXT_DISPLAY_START) {
        static sf::Text wastedText;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            fontLoaded = true;
            wastedText.setFont(ResourceManager::getFont("fonts/font4.ttf"));
            wastedText.setString("wasted");
            wastedText.setCharacterSize(150);
            wastedText.setFillColor({255, 199, 0});
            // wastedText.setOutlineColor(sf::Color::Black);
            // wastedText.setOutlineThickness(0.4f);
            wastedText.setStyle(sf::Text::Bold);
            sf::FloatRect textBounds = wastedText.getLocalBounds();
            wastedText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
        }
        wastedText.setPosition(player.getPosition().x, player.getPosition().y - VIEW_HEIGHT * 0.2f);
        float scale = 0.008f * window.getSize().x / wastedText.getLocalBounds().width;
        wastedText.setScale(scale, scale);
        window.draw(wastedText);
    }
    renderDialogueSystem(window);
    renderPauseMenu(window);
}
void LevelSceneBase::handleEntityStaticData() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_STATIC_DATA)) {
        // if (packet->size() % 8 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.canRead(9)) {
            Entity entityId = reader.nextUInt16();
            auto type = static_cast<EntityTypeID>(reader.nextUInt8());
            if (type >= EntityTypeID::COUNT) break;  //bad type
            std::uint8_t netSize = reader.nextUInt8();
            std::uint16_t netX = reader.nextUInt16();
            std::uint16_t netY = reader.nextUInt16();
            bool hasNameTag = reader.nextUInt8() != 0;
            char nameTag[16];
            if (hasNameTag) {
                if (!reader.canRead(16)) break;  //bad packet
                for (int i = 0; i < 16; ++i) {
                    nameTag[i] = static_cast<char>(reader.nextUInt8());
                }
            }
            if (type == EntityTypeID::FOOD_BALL){
                entities_[entityId] = std::make_unique<ClientFoodBall>(sf::Vector2f(ntolX(netX), ntolY(netY)));
                entities_[entityId]->setType(type);
                float lightRadius = LightingSystem::getLightRadius(type);
                if (lightRadius > 0.f) {
                    if (const sf::Uint8* lightColorRGB = LightingSystem::getLightColor(type)) {
                        entities_[entityId]->setLightProps(LightingSystem::getLightRadius(type),
                           sf::Color(lightColorRGB[0], lightColorRGB[1], lightColorRGB[2]));
                    }
                }
            }
            else {
                auto& entity = entities_[entityId];   //will create if not exist, replace if exist
                entity = std::make_unique<NetworkEntity>();
                entity->setType(type);
                entity->setSize(ntolSize8(netSize));
                entity->setNetworkState({ntolX(netX), ntolY(netY)}, getCurrentTick());
                float lightRadius = LightingSystem::getLightRadius(type);
                if (lightRadius > 0.f) {
                    if (const sf::Uint8* lightColorRGB = LightingSystem::getLightColor(type)) {
                        entities_[entityId]->setLightProps(LightingSystem::getLightRadius(type),
                           sf::Color(lightColorRGB[0], lightColorRGB[1], lightColorRGB[2]));
                    }
                }
            }
            if (hasNameTag)
                entities_[entityId]->setNameTag(nameTag);
        }
    }
}
void LevelSceneBase::handleEntityLeave() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_LEAVE)) {
        if (packet->size() % 2 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            entities_.erase(entityId);
        }
    }
}
void LevelSceneBase::handleEntityDynamic() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_DYNAMIC_DATA)) {
        if (packet->size() % 6 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            std::uint16_t netX = reader.nextUInt16();
            std::uint16_t netY = reader.nextUInt16();
            auto it = entities_.find(entityId);
            if (it != entities_.end()) {
                it->second->setNetworkState({ntolX(netX), ntolY(netY)}, getCurrentTick());
            }
        }
    }
}
void LevelSceneBase::handleEntitySizeChange() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_SIZE_CHANGE)) {
        if (packet->size() % 3 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            std::uint8_t netSize = reader.nextUInt8();
            auto it = entities_.find(entityId);
            if (it != entities_.end()) {
                it->second->setSize(ntolSize8(netSize));
            }
        }
    }
}
void LevelSceneBase::handlePlayerAttributesUpdate() {
    while (auto packet = driver_->popPacket(PacketType::PKT_PLAYER_ATTRIBUTES_UPDATE)) {
        if (packet->size() != 8) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        std::uint16_t netMaxHP = reader.nextUInt16();
        std::uint16_t netMaxFP = reader.nextUInt16();
        std::uint16_t netMaxVec = reader.nextUInt16();
        std::uint16_t netMaxAcc = reader.nextUInt16();
        playerAttributes_.maxHP = ntolHP16(netMaxHP);  //update player attributes for other levels to use
        playerAttributes_.maxFP = ntolFP(netMaxFP);
        playerAttributes_.maxVec = ntolVec(netMaxVec);
        playerAttributes_.maxAcc = ntolAcc(netMaxAcc);
        player.setMaxVec(playerAttributes_.maxVec);
        player.setMaxAcc(playerAttributes_.maxAcc);
        playerStatus_.setMaxHP(playerAttributes_.maxHP);
        playerStatus_.setMaxFP(playerAttributes_.maxFP);
    }
}
void LevelSceneBase::handleMessagePacket() {
    while (auto packet = driver_->popPacket(ClientTypes::PacketType::PKT_MESSAGE)) {
        PacketReader reader(std::move(*packet));
        std::string message = reader.nextStr();
        chatBox_->addMessage(message);
    }
}
void LevelSceneBase::handlePlayerDash() {
    while (auto packet = driver_->popPacket(ClientTypes::PacketType::PKT_PLAYER_DASH)) {
        PacketReader reader(std::move(*packet));
        std::uint16_t netDashVel = reader.nextUInt16();
        float dashVel = ntolVec(netDashVel);
        player.dash(dashVel);
    }
}
void LevelSceneBase::handleSkillPackets() {
    while (auto packet = driver_->popPacket(ClientTypes::PacketType::PKT_SKILL_APPLIED)) {
        PacketReader reader(std::move(*packet));
        std::uint8_t relativeSkillIndex = reader.nextUInt8();
        if (relativeSkillIndex > 3) continue;  //bad packet
        skillBar_.setSkillActive(relativeSkillIndex, true);
        skillBar_.setSkillColorful(relativeSkillIndex, true);
    }
    while (auto packet = driver_->popPacket(ClientTypes::PacketType::PKT_SKILL_END)) {
        PacketReader reader(std::move(*packet));
        std::uint8_t relativeSkillIndex = reader.nextUInt8();
        if (relativeSkillIndex > 3) continue;  //bad packet
        skillBar_.setSkillActive(relativeSkillIndex, false);
        skillBar_.setSkillColorful(relativeSkillIndex, false);
    }
    while (auto packet = driver_->popPacket(ClientTypes::PacketType::PKT_SKILL_READY)) {
        PacketReader reader(std::move(*packet));
        std::uint8_t relativeSkillIndex = reader.nextUInt8();
        if (relativeSkillIndex > 3) continue;  //bad packet
        skillBar_.setSkillActive(relativeSkillIndex, false);
        skillBar_.setSkillColorful(relativeSkillIndex, true);
    }
}
void LevelSceneBase::handleEntityDeath() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_DEATH)) {
        if (packet->size() % 2 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            auto it = entities_.find(entityId);
            if (it != entities_.end()) {
                if (it->second->getType() != EntityTypeID::FOOD_BALL) {
                    deathSystem_.spawnDeathEffect(
                        it->second->getSprite(),
                        it->second->getVelocity(),
                        std::abs(it->second->getSprite().getScale().x)
                    );
                    entities_.erase(it);
                }
                else {
                    // sf::Vector2f pos = it->second->getPosition();
                    // constexpr static float FOOD_BALL_ANIM_DIS_THRESHOLD = 5.f;
                    // if (std::abs(pos.x - player.getPosition().x) < FOOD_BALL_ANIM_DIS_THRESHOLD + player.getSize() / 2.f &&
                    //     std::abs(pos.y - player.getPosition().y) < FOOD_BALL_ANIM_DIS_THRESHOLD + player.getSize() / 2.f) {
                    reinterpret_cast<ClientFoodBall*>(it->second.get())->startAbsorb();
                    // }
                }
            }
        }
    }
}
void LevelSceneBase::handleEntityHPChange() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_HP_CHANGE)) {
        if (packet->size() % 3 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            std::uint8_t netHP = reader.nextUInt8();
            auto it = entities_.find(entityId);
            if (it != entities_.end()) {
                it->second->setHpPercentage(netHP / 255.f);
            }
        }
    }
}
void LevelSceneBase::spawnDamagePopup(float delta, const sf::Vector2f& pos) {
    DamagePopup popup;
    popup.maxLifetime = 2.0f;
    popup.lifetime = popup.maxLifetime;
    popup.text.setFont(ResourceManager::getFont("fonts/font6.ttf"));
    popup.text.setString(std::format("{:.1f}", std::abs(delta)));
    popup.text.setCharacterSize(40);
    popup.text.setStyle(sf::Text::Bold);
    if (delta >= 0) {
        popup.initialColor = sf::Color::Green;
    } else {
        //yellow(255,255,0) to red(255,0,0)
        float damage = std::abs(delta);
        float ratio = std::clamp(damage / 200.f, 0.f, 1.f);
        auto green = static_cast<sf::Uint8>(255.f * (1.f - ratio));
        popup.initialColor = sf::Color(255, green, 0);
    }
    popup.text.setFillColor(popup.initialColor);
    // popup.text.setOutlineColor(sf::Color::Black);
    // popup.text.setOutlineThickness(2.0f);
    sf::FloatRect bounds = popup.text.getLocalBounds();
    popup.text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    popup.text.setPosition(pos.x, pos.y - 0.6f);
    float scale = 0.01f;
    popup.text.setScale(scale, scale);
    damagePopups_.push_back(popup);
}
void LevelSceneBase::updateDamagePopups(float dt) {
    for (auto it = damagePopups_.begin(); it != damagePopups_.end();) {
        it->lifetime -= dt;
        if (it->lifetime <= 0.f) {
            it = damagePopups_.erase(it);
        } else {
            float alphaRatio = std::min(3.f * it->lifetime / it->maxLifetime, 1.f);
            auto alpha = static_cast<sf::Uint8>(255 * alphaRatio);
            sf::Color color = it->initialColor;
            color.a = alpha;
            it->text.setFillColor(color);
            sf::Color outline = sf::Color::Black;
            outline.a = alpha;
            it->text.setOutlineColor(outline);
            it->text.move(0.f, -0.3f * dt);
            ++it;
        }
    }
}
void LevelSceneBase::handleHPDelta() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_HP_DELTA)) {
        if (packet->size() % 4 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            float delta = ntolHPDelta(reader.nextUInt16());
            auto it = entities_.find(entityId);
            if (it != entities_.end()) {
                spawnDamagePopup(delta, it->second->getPosition());
                it->second->triggerHurtFlash(1 - (1 / (std::abs(delta) * 32.f + 1)));
            }
        }
    }
}
void LevelSceneBase::handlePlayerRespawn() {
    while (auto packet = driver_->popPacket(PacketType::PKT_PLAYER_RESPAWN)) {
        state_ = State::DEATH;
        deathTimer_ = 0.f;
        deathSystem_.spawnDeathEffect(
                    player.getSprite(),
                    player.getVelocity(),
                    std::abs(player.getSprite().getScale().x)
                );
        pauseMenu_.active = false;
        dialogueSystem_.active = false;
    }
}
void LevelSceneBase::update(float dt) {
    ++currentTick_;
    driver_->pollPacket();
    if (state_ != State::DEATH) {
        handlePlayerAttributesUpdate();
        handlePlayerStateUpdate();
    }
    handleHPDelta();
    handleEntityLeave();
    handleEntityStaticData();
    handleEntityDynamic();
    handleEntityDeath();
    handleEntityHPChange();
    handleEntitySizeChange();
    handleMessagePacket();
    handlePlayerDash();
    handleSkillPackets();
    handlePlayerRespawn();
    handleDialoguePacket();
    sf::Vector2f mouseWorld = InputManager::getInstance().mousePosWorld;
    updatePauseMenu(dt, mouseWorld);
    updateDialogueSystem(dt);
    //move view toward player position
    view_.move(GameData::CAMERA_ALPHA * (player.getPosition() - view_.getCenter()));
    correctView();
    if (state_ == State::GAMING) {
        for (auto& pair : entities_) {
            pair.second->update(dt);
        }
        sf::Vector2f playerRawAcc {};  // without clamp
        bool inputBlocked = pauseMenu_.active;// || dialogueSystem_.active || chatBox_->isOpen();
        if (!inputBlocked && InputManager::getInstance().mousePressedLeft) {
            sf::Vector2f dir = mouseWorld - player.getPosition();
            playerRawAcc = accDisRatio_ * dir;
        }
        player.update(dt, playerRawAcc);
        playerStatus_.update(dt);
        //send player pos packet to server
        if (currentTick_ % TICKS_PER_PLAYER_TRANSFORM_UPLOAD == 0) {
            writer_.writeInt16(ltonX(player.getPosition().x))
               .writeInt16(ltonY(player.getPosition().y));
            driver_->send(writer_.takePacket(), 0, ServerTypes::PacketType::PKT_TRANSFORM, false);
            writer_.clearBuffer();
        }
    }
    else if (state_ == State::DEATH) {
        deathTimer_ += dt;
        if (deathTimer_ > DEATH_TEXT_DISPLAY_END) {
            state_ = State::GAMING;
            deathTimer_ = 0.f;
            viewInit_ = false;
            player.setPos(Random::randFloat(0.f, getMapSize().x), Random::randFloat(0.f, getMapSize().y));
        }
    }
    deathSystem_.update(dt * (state_ == State::DEATH ? 0.5f : 1.f));
    chatBox_->update(dt);
    skillBar_.update();
    driver_->flush();  //important
    for (auto& food : foodBalls_) {
        food.update(dt);
    }
    for (auto it = entities_.begin(); it != entities_.end(); ) {  //clean dead entities
        if (it->second->isDead()) {
            it = entities_.erase(it);
        } else {
            ++it;
        }
    }
    lightingSystem_.update(dt);
    updateDamagePopups(dt);
}
void LevelSceneBase::initPauseMenu() {
    pauseMenu_.background.setFillColor(sf::Color(0, 0, 0, 150));
    auto loadBtn = [&](PauseMenu::Button& btn, std::string path, std::function<void()> cb) {
        btn.sprite.setTexture(ResourceManager::getTexture(path));
        sf::FloatRect bounds = btn.sprite.getLocalBounds();
        btn.sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        float scale = 5.0f / bounds.height;
        btn.baseScale = {scale, scale};
        btn.sprite.setScale(btn.baseScale);
        btn.onClick = cb;
    };
    loadBtn(pauseMenu_.btnBack, "images/ui/btn_back.png", [this]() {
        writer_.writeInt8(static_cast<std::uint8_t>(0));  //to level 0
        driver_->send(writer_.takePacket(), 0, ServerTypes::PacketType::PKT_LEVEL_CHANGE, 1);
        writer_.clearBuffer();
        SceneSwitchRequest request = {
            SceneSwitchRequest::Pop,
            nullptr,
            0,
            0
        };
        onRequestSwitch_(request);
    });
    loadBtn(pauseMenu_.btnRetry, "images/ui/btn_retry.png", [this]() {
        pauseMenu_.active = false;
        state_ = State::DEATH;
        deathTimer_ = 0.f;
        deathSystem_.spawnDeathEffect(player.getSprite(), player.getVelocity(), std::abs(player.getSprite().getScale().x));
    });
    loadBtn(pauseMenu_.btnResume, "images/ui/btn_resume.png", [this]() {
        pauseMenu_.active = false;
    });
}
void LevelSceneBase::updatePauseMenu(float dt, const sf::Vector2f& mouseWorldPos) {
    if (!pauseMenu_.active) return;
    pauseMenu_.background.setSize(view_.getSize());
    pauseMenu_.background.setOrigin(view_.getSize() / 2.f);
    pauseMenu_.background.setPosition(view_.getCenter());
    float centerY = view_.getCenter().y + pauseMenu_.BUTTON_Y_OFFSET;
    auto updateBtn = [&](PauseMenu::Button& btn, float xOffset) {
        btn.sprite.setPosition(view_.getCenter().x + xOffset, centerY);
        bool hovered = btn.sprite.getGlobalBounds().contains(mouseWorldPos);
        float targetRatio = hovered ? 1.2f : 1.0f;
        btn.currentScaleRatio += (targetRatio - btn.currentScaleRatio) * dt * 10.f;
        btn.sprite.setScale(btn.baseScale * btn.currentScaleRatio);
    };
    updateBtn(pauseMenu_.btnBack, -10.f);
    updateBtn(pauseMenu_.btnRetry, 0.f);
    updateBtn(pauseMenu_.btnResume, 10.f);
}
void LevelSceneBase::renderPauseMenu(sf::RenderWindow& window) const {
    if (!pauseMenu_.active) return;
    window.setView(view_);
    window.draw(pauseMenu_.background);
    window.draw(pauseMenu_.btnBack.sprite);
    window.draw(pauseMenu_.btnRetry.sprite);
    window.draw(pauseMenu_.btnResume.sprite);
}
void LevelSceneBase::handlePauseInput(const sf::Event& event, const sf::Vector2f& mouseWorldPos) {
    if (!pauseMenu_.active) return;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        auto checkClick = [&](PauseMenu::Button& btn) {
            if (btn.sprite.getGlobalBounds().contains(mouseWorldPos)) {
                if (btn.onClick) btn.onClick();
            }
        };
        checkClick(pauseMenu_.btnBack);
        checkClick(pauseMenu_.btnRetry);
        checkClick(pauseMenu_.btnResume);
    }
}
void LevelSceneBase::initDialogueSystem() {
    dialogueSystem_.bgBox.setFillColor(sf::Color(0, 0, 0, 128)); // 灰色半透明
    dialogueSystem_.textDrawable.setFont(ResourceManager::getFont("fonts/font4.ttf")); // 确保字体存在
    dialogueSystem_.textDrawable.setCharacterSize(24);
    dialogueSystem_.textDrawable.setFillColor(sf::Color::White);
    dialogueSystem_.localDatabase[1] = {
        {"Hello, %PLAYER_NAME%. Welcome to the deep sea.", "audio/s_tip.wav"},
        {"The pressure here is immense...", "audio/s_tip.wav"},
        {"But you seem to be adapting well.", "audio/s_tip.wav"}
    };
}
std::string LevelSceneBase::parseDialogueText(const std::string& raw) {
    std::string res = raw;
    std::string placeholder = "%PLAYER_NAME%";
    std::string playerName = GameData::playerId[0] == '@' ? "Player" : GameData::playerId;
    size_t pos = 0;
    while ((pos = res.find(placeholder, pos)) != std::string::npos) {
        res.replace(pos, placeholder.length(), playerName);
        pos += playerName.length();
    }
    return res;
}
void LevelSceneBase::startDialogue(int index, bool canClose) {
    dialogueSystem_.active = true;
    dialogueSystem_.canCloseOnEnter = canClose;
    dialogueSystem_.currentDialogueIndex = index;
    dialogueSystem_.currentLineIndex = 0;
    if (dialogueSystem_.localDatabase.contains(index)) {  //safety check
        dialogueSystem_.currentLines = dialogueSystem_.localDatabase[index];
    } else {
        dialogueSystem_.currentLines = { {"Dialogue index " + std::to_string(index) + " not found.", ""} };
    }
    if (!dialogueSystem_.currentLines.empty()) {
        dialogueSystem_.fullTargetString = parseDialogueText(dialogueSystem_.currentLines[0].text);
        dialogueSystem_.displayString = "";
        dialogueSystem_.charIndex = 0;
        dialogueSystem_.typeTimer = 0.f;
    }
}
void LevelSceneBase::advanceDialogue() {
    if (dialogueSystem_.charIndex < dialogueSystem_.fullTargetString.length()) {
        dialogueSystem_.displayString = dialogueSystem_.fullTargetString;
        dialogueSystem_.charIndex = dialogueSystem_.fullTargetString.length();
        return;
    }
    if (dialogueSystem_.currentLineIndex + 1 < dialogueSystem_.currentLines.size()) {
        dialogueSystem_.currentLineIndex++;
        dialogueSystem_.fullTargetString = parseDialogueText(dialogueSystem_.currentLines[dialogueSystem_.currentLineIndex].text);
        dialogueSystem_.displayString = "";
        dialogueSystem_.charIndex = 0;
        dialogueSystem_.typeTimer = 0.f;
    }
    else {
        if (dialogueSystem_.canCloseOnEnter) {
            dialogueSystem_.active = false;
            //maybe send packet to server, but all-client is okay
        }
    }
}
void LevelSceneBase::updateDialogueSystem(float dt) {
    if (!dialogueSystem_.active) return;
    if (dialogueSystem_.charIndex < dialogueSystem_.fullTargetString.length()) {
        dialogueSystem_.typeTimer += dt;
        if (dialogueSystem_.typeTimer >= dialogueSystem_.TYPE_INTERVAL) {
            dialogueSystem_.typeTimer = 0.f;
            dialogueSystem_.displayString += dialogueSystem_.fullTargetString[dialogueSystem_.charIndex];
            ++dialogueSystem_.charIndex;
            AudioManager::getInstance().playSound(dialogueSystem_.currentLines[dialogueSystem_.currentLineIndex].soundName);
        }
    }
}
void LevelSceneBase::renderDialogueSystem(sf::RenderWindow& window) {
    if (!dialogueSystem_.active) return;
    window.setView(view_);
    float w = view_.getSize().x;
    float h = view_.getSize().y;
    float boxH = h * 0.08f;
    dialogueSystem_.bgBox.setSize({w * 0.95f, boxH});
    dialogueSystem_.bgBox.setOrigin(w * 0.45f, boxH);
    dialogueSystem_.bgBox.setPosition(view_.getCenter().x, view_.getCenter().y + h / 2.f - h * 0.05f);
    // window.draw(dialogueSystem_.bgBox);
    float textScale = 0.12f * (view_.getSize().x / 80.f);
    dialogueSystem_.textDrawable.setScale(textScale, textScale);
    dialogueSystem_.textDrawable.setString(dialogueSystem_.displayString);
    sf::Vector2f boxPos = dialogueSystem_.bgBox.getPosition();
    sf::Vector2f boxOrigin = dialogueSystem_.bgBox.getOrigin();
    float boxLeft = boxPos.x - boxOrigin.x;
    float boxTop = boxPos.y - boxOrigin.y;
    dialogueSystem_.textDrawable.setPosition(boxLeft + 0.2f, boxTop + 0.2f);
    dialogueSystem_.textDrawable.setFillColor({63, 63, 63});
    window.draw(dialogueSystem_.textDrawable);
    dialogueSystem_.textDrawable.setPosition(boxLeft, boxTop);
    dialogueSystem_.textDrawable.setFillColor({255, 255, 255});
    window.draw(dialogueSystem_.textDrawable);
}
void LevelSceneBase::handleDialoguePacket() {
    while (auto packet = driver_->popPacket(PacketType::PKT_DIALOGUE_EVENT)) {
        PacketReader reader(std::move(*packet));
        bool isOpen = reader.nextUInt8();
        if (isOpen) {
            std::uint16_t index = reader.nextUInt16();
            bool canClose = reader.nextUInt8();
            startDialogue(index, canClose);
        } else {
            dialogueSystem_.active = false;
        }
    }
}