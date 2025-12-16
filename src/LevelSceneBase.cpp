#include "client/scenes/levelscenes/LevelSceneBase.h"

LevelSceneBase::LevelSceneBase(const std::shared_ptr<ClientNetworkDriver>& driver,
    ClientCommonPlayerAttributes& playerAttributes, const std::shared_ptr<ChatBox>& chatBox)
    : driver_(driver), skillBar_(*driver), playerAttributes_(playerAttributes), chatBox_(chatBox) {
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
}

void LevelSceneBase::handlePlayerStateUpdate() {
    while (auto packet = driver_->popPacket(PacketType::PKT_PLAYER_STATE_UPDATE)) {
        if (driver_->hasPacket(PKT_PLAYER_STATE_UPDATE)) continue;  //only handle latest packet
        if (packet->size() != 4) continue;  //wrong packet
        PacketReader reader(std::move(*packet));
        std::uint16_t netHP = reader.nextUInt16();
        std::uint16_t netFP = reader.nextUInt16();
        float hp = ntolHP16(netHP);
        float fp = ntolFP(netFP);
        playerStatus_.setHP(hp);
        playerStatus_.setFP(fp);
    }
}
void LevelSceneBase::render(sf::RenderWindow &window) {
    if (!viewInit_) {
        resetViewSize(window.getSize().x, window.getSize().y);
        correctView();
        viewInit_ = true;
    }
    window.setView(view_);
    window.draw(background_);
    for (auto& pair : entities_) {
        pair.second.render(window);
    }
    player.render(window);
    //render player status UI
    playerStatus_.render(window);
    chatBox_->render(window);
    skillBar_.render(window);
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
void LevelSceneBase::update(float dt) {
    ++currentTick_;
    driver_->pollPacket();
    handlePlayerAttributesUpdate();
    handlePlayerStateUpdate();
    handleEntityLeave();
    handleEntityStaticData();
    handleEntityDynamic();
    handleEntitySizeChange();
    handleMessagePacket();
    handlePlayerDash();
    handleSkillPackets();
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
    //move view toward player position
    view_.move(GameData::CAMERA_ALPHA * (player.getPosition() - view_.getCenter()));
    correctView();
    //send player pos packet to server
    if (currentTick_ % TICKS_PER_PLAYER_TRANSFORM_UPLOAD == 0) {
        writer_.writeInt16(ltonX(player.getPosition().x))
           .writeInt16(ltonY(player.getPosition().y));
        driver_->send(writer_.takePacket(), 0, ServerTypes::PacketType::PKT_TRANSFORM, false);
        writer_.clearBuffer();
    }
    chatBox_->update(dt);
    skillBar_.update();
}
