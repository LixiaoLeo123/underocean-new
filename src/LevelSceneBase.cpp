#include "client/scenes/levelscenes/LevelSceneBase.h"

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
    for (auto& pair : entities_) {
        pair.second.render(window);
    }
    if (state_ != State::DEATH)
        player.render(window);
    deathSystem_.render(window);
    //light
    lightingSystem_.clear(ambientLightColor_);
    static std::vector<sf::FloatRect> obstacles;
    obstacles.clear();
    for (auto& pair : entities_) {
        obstacles.push_back(pair.second.getSprite().getGlobalBounds());
    }
    lightingSystem_.updateObstacles(obstacles);
    for (auto& pair : entities_) {
        if (pair.second.hasLight()) {
            lightingSystem_.drawLight(
                pair.second.getPosition(),
                view_,
                pair.second.getLightRadius(),
                pair.second.getLightColor()
            );
        }
    }
    lightingSystem_.display();
    sf::View currentView = window.getView();
    window.setView(window.getDefaultView());
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
            wastedText.setStyle(sf::Text::Bold);
            sf::FloatRect textBounds = wastedText.getLocalBounds();
            wastedText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
        }
        wastedText.setPosition(player.getPosition().x, player.getPosition().y - VIEW_HEIGHT * 0.2f);
        float scale = 0.008f * window.getSize().x / wastedText.getLocalBounds().width;
        wastedText.setScale(scale, scale);
        window.draw(wastedText);
    }
}
void LevelSceneBase::handleEntityStaticData() {
    while (auto packet = driver_->popPacket(PacketType::PKT_ENTITY_STATIC_DATA)) {
        if (packet->size() % 8 != 0) continue;  //bad packet
        PacketReader reader(std::move(*packet));
        while (reader.hasNext()) {
            Entity entityId = reader.nextUInt16();
            auto type = static_cast<EntityTypeID>(reader.nextUInt8());
            if (type >= EntityTypeID::COUNT) continue;  //bad type
            std::uint8_t netSize = reader.nextUInt8();
            std::uint16_t netX = reader.nextUInt16();
            std::uint16_t netY = reader.nextUInt16();
            auto& entity = entities_[entityId];   //will create if not exist, replace if exist
            entity.setType(type);
            entity.setSize(ntolSize8(netSize));
            entity.setNetworkState({ntolX(netX), ntolY(netY)}, getCurrentTick());
            float lightRadius = LightingSystem::getLightRadius(type);
            if (lightRadius <= 0.f) continue;
            const sf::Uint8* lightColorRGB = LightingSystem::getLightColor(type);
            if (lightColorRGB == nullptr) continue;
            entity.setLightProps(LightingSystem::getLightRadius(type),
                sf::Color(lightColorRGB[0], lightColorRGB[1], lightColorRGB[2]));
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
                it->second.setNetworkState({ntolX(netX), ntolY(netY)}, getCurrentTick());
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
                it->second.setSize(ntolSize8(netSize));
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
                deathSystem_.spawnDeathEffect(
                    it->second.getSprite(),
                    it->second.getVelocity(),
                    std::abs(it->second.getSprite().getScale().x)
                );
                entities_.erase(it);
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
                it->second.setHpPercentage(netHP / 255.f);
            }
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
    }
}
void LevelSceneBase::update(float dt) {
    ++currentTick_;
    driver_->pollPacket();
    if (state_ != State::DEATH) {
        handlePlayerAttributesUpdate();
        handlePlayerStateUpdate();
    }
    handleEntityLeave();
    handleEntityStaticData();
    handleEntityDynamic();
    handleEntityDeath();
    handleEntityHPChange();
    handleEntitySizeChange();
    handleHPDelta();
    handleMessagePacket();
    handlePlayerDash();
    handleSkillPackets();
    handlePlayerRespawn();
    if (state_ == State::GAMING) {
        for (auto& pair : entities_) {
            pair.second.update(dt);
        }
        sf::Vector2f playerRawAcc {};  // without clamp
        if (InputManager::getInstance().mousePressedLeft) {
            sf::Vector2f mouseWorld = InputManager::getInstance().mousePosWorld;
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
    //move view toward player position
    view_.move(GameData::CAMERA_ALPHA * (player.getPosition() - view_.getCenter()));
    correctView();
    deathSystem_.update(dt * (state_ == State::DEATH ? 0.5f : 1.f));
    chatBox_->update(dt);
    skillBar_.update();
    driver_->flush();  //important
    lightingSystem_.update(dt);
}
