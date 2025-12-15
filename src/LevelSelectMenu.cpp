#include "client/scenes/LevelSelectMenu/LevelSelectMenu.h"

#include "client/common/ChatBox.h"
#include "client/common/ResourceManager.h"
#include "client/scenes/levelscenes/LevelScene1.h"
#include "server/core(deprecate)/GameData.h"
#include "server/new/levels/Level0.h"
#define LEVEL_BUTTON_SIZE sf::Vector2f(WIDTH * 0.3f,HEIGHT * 0.35f)
LevelSelectMenu::LevelSelectMenu(const std::shared_ptr<SmoothTextLabel> &title, std::string ip, int port)  //from start menu
    : title_(title), LazyPanelScene(WIDTH, HEIGHT), networkDriver_(std::make_shared<ClientNetworkDriver>()),
      levelButtons_{
          {
              std::make_shared<ImageButton>(ResourceManager::getTexture("images/icons/glevel1.png"),
                                            ResourceManager::getTexture("images/icons/level1.png"),
                                            sf::Vector2f(WIDTH * 0.02f, HEIGHT * 0.21f), LEVEL_BUTTON_SIZE, 1),
              std::make_shared<ImageButton>(ResourceManager::getTexture("images/icons/glevel2.png"),
                                            ResourceManager::getTexture("images/icons/level2.png"),
                                            sf::Vector2f(WIDTH * 0.35f, HEIGHT * 0.21f), LEVEL_BUTTON_SIZE, 2),
              std::make_shared<ImageButton>(ResourceManager::getTexture("images/icons/glevel3.png"),
                                            ResourceManager::getTexture("images/icons/level3.png"),
                                            sf::Vector2f(WIDTH * 0.68f, HEIGHT * 0.21f), LEVEL_BUTTON_SIZE, 3),
              std::make_shared<ImageButton>(ResourceManager::getTexture("images/icons/glevel4.png"),
                                            ResourceManager::getTexture("images/icons/level4.png"),
                                            sf::Vector2f(WIDTH * 0.02f, HEIGHT * 0.62f), LEVEL_BUTTON_SIZE, 4),
              std::make_shared<ImageButton>(ResourceManager::getTexture("images/icons/glevel5.png"),
                                            ResourceManager::getTexture("images/icons/level5.png"),
                                            sf::Vector2f(WIDTH * 0.35f, HEIGHT * 0.62f), LEVEL_BUTTON_SIZE, 5),
              std::make_shared<ImageButton>(ResourceManager::getTexture("images/icons/glevel6.png"),
                                            ResourceManager::getTexture("images/icons/level6.png"),
                                            sf::Vector2f(WIDTH * 0.68f, HEIGHT * 0.62f), LEVEL_BUTTON_SIZE, 6)
          }
      },
      obj0_(ResourceManager::getTexture("images/backgrounds/bg3/stars.png"), HEIGHT, -0.2f, 0.f, 10.f, 0.06f),
      obj1_(ResourceManager::getTexture("images/backgrounds/bg3/ball.png"), HEIGHT, 0.1f, 1.f, 10.f, 0.06f),
      chatBox_(std::make_shared<ChatBox>(*networkDriver_, std::string(GameData::playerId)))
{
    if (GameData::currentLevel < 4) {
        background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg2/bg2.png"));
        obj0_ = FloatingObj(ResourceManager::getTexture("images/backgrounds/bg2/cloud.png"), HEIGHT, -0.06f, 0.f,
                            10.f, 0.02f);
        obj1_ = FloatingObj(ResourceManager::getTexture("images/backgrounds/bg2/sea.png"), HEIGHT, 0.06f, 1.f, 10.f,
                            0.04f);
    } else {
        background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg3/bg3.png"));
        advanced = true;
    }
    for (int i = GameData::currentLevel; i < 6; ++i) {            //grey for locked
        levelButtons_[i]->setTexture(
            ResourceManager::getTexture("images/icons/glevel" + std::to_string(i + 1) + ".png"),
            ResourceManager::getTexture("images/icons/glevel" + std::to_string(i + 1) + ".png"));
    }
    for (int i = 0; i < GameData::currentLevel; ++i) {              //add callback
        levelButtons_[i]->setOnClick([this](int levelNum) {
            this->handleLevelButtonClick(levelNum);
        });
    }
    for (int i = 0; i < 6; ++i) add(levelButtons_[i]);
    if (!networkDriver_->connect(ip, port)) throw std::runtime_error("Error building connection");  //do something
    networkDriver_->setOnConnect([this](){
        this->handleConnect();
    });
    networkDriver_->setOnDisconnect([this](){
        this->handleDisconnect();
    });
}
void LevelSelectMenu::reloadUI() {
    if (GameData::currentLevel < 4) {
        background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg2/bg2.png"));
        obj0_ = FloatingObj(ResourceManager::getTexture("images/backgrounds/bg2/cloud.png"), HEIGHT, -0.06f, 0.f,10.f, 0.02f);
        obj1_ = FloatingObj(ResourceManager::getTexture("images/backgrounds/bg2/sea.png"), HEIGHT, 0.06f, 1.f, 10.f,0.04f);
        advanced = false;
    } else {
        background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg3/bg3.png"));
        obj0_ = FloatingObj(ResourceManager::getTexture("images/backgrounds/bg3/stars.png"), HEIGHT, -0.2f, 0.f, 10.f, 0.06f);
        obj1_ = FloatingObj(ResourceManager::getTexture("images/backgrounds/bg3/ball.png"), HEIGHT, 0.1f, 1.f, 10.f, 0.06f);
        advanced = true;
    }
    for (int i = 0; i < GameData::currentLevel; ++i) {            //grey for locked
        levelButtons_[i]->setTexture(
            ResourceManager::getTexture("images/icons/glevel" + std::to_string(i + 1) + ".png"),
            ResourceManager::getTexture("images/icons/glevel" + std::to_string(i + 1) + ".png"));
    }
    for (int i = GameData::currentLevel; i < 6; ++i) {            //grey for locked
        levelButtons_[i]->setTexture(
            ResourceManager::getTexture("images/icons/glevel" + std::to_string(i + 1) + ".png"),
            ResourceManager::getTexture("images/icons/level" + std::to_string(i + 1) + ".png"));
    }
    for (int i = 0; i < 6; ++i) {  //add callback
        if (i < GameData::currentLevel) {
            levelButtons_[i]->setOnClick([this](int levelNum) {
                this->handleLevelButtonClick(levelNum);
            });
        }
        else {
            levelButtons_[i]->setOnClick(nullptr);   //do nothing
        }
    }
}
void LevelSelectMenu::handleFinishLoginPacket() {
    if (auto packet = networkDriver_->popPacket(ClientTypes::PacketType::PKG_FINISH_LOGIN)) {
        if (packet->size() != 12) return;
        PacketReader reader(std::move(*packet));
        playerAttributes_.maxHP = ntolHP16(reader.nextUInt16());
        playerAttributes_.maxFP = ntolFP(reader.nextUInt16());
        playerAttributes_.maxVec = ntolVec(reader.nextUInt16());
        playerAttributes_.maxAcc = ntolAcc(reader.nextUInt16());
        playerAttributes_.skillIndices[0] = reader.nextUInt8();
        playerAttributes_.skillIndices[1] = reader.nextUInt8();
        playerAttributes_.skillIndices[2] = reader.nextUInt8();
        playerAttributes_.skillIndices[3] = reader.nextUInt8();
        statusIndicator_.setStateConnected();
        hasLogin = true;
    }
}
void LevelSelectMenu::handleMessagePacket() {
    while (auto packet = networkDriver_->popPacket(ClientTypes::PacketType::PKT_MESSAGE)) {
        PacketReader reader(std::move(*packet));
        std::string message = reader.nextStr();
        chatBox_->addMessage(message);
    }
}
void LevelSelectMenu::update(float dt) {
    if (shouldReloadUI) {
        reloadUI();
        shouldReloadUI = false;
    }
    obj0_.update(dt);
    obj1_.update(dt);
    title_->updateTotal(dt);
    statusIndicator_.update(dt);
    networkDriver_->pollPacket();   //packet handle start (may call on(dis)connect)
    handleFinishLoginPacket();
    handleMessagePacket();
    chatBox_->update(dt);
}
void LevelSelectMenu::render(sf::RenderWindow &window) {
    if (advanced) {
        window.draw(background_);
        obj0_.render(window);
        obj1_.render(window);
    }else {
        obj0_.render(window);
        window.draw(background_);
        obj1_.render(window);
    }
    LazyPanelScene::render(window);
    getPanel().render(window);
    title_->render(window);
    statusIndicator_.render(window);
    chatBox_->render(window);
}
void LevelSelectMenu::handleEvent(const sf::Event &event) {
    LazyPanelScene::handleEvent(event);
    if (!chatBox_->isOpen())
        getPanel().handleEvent(event);
    statusIndicator_.adjustBound(view_);
    chatBox_->handleEvent(event);
    // if (event.type == sf::Event::Resized) {
    //     unsigned int winWidth = event.size.width;
    //     unsigned int winHeight = event.size.height;
    //     resetViewArea(winWidth, winHeight);
    //     selectPanel_.setBounds(view_.getCenter() - view_.getSize() / 2.f, view_.getSize());
    // }
    // selectPanel_.handleEvent(event);
}
void LevelSelectMenu::handleConnect() {
    sendLoginPacket();
}
void LevelSelectMenu::handleDisconnect() {
    statusIndicator_.setStateConnecting();
}
void LevelSelectMenu::sendLoginPacket() {
    writer_.writeStr(GameData::playerId, 16)
        .writeInt8(static_cast<std::uint8_t>(GameData::playerType))
        .writeInt16(ltonSize16(GameData::playerSize[GameData::playerType]))
        .writeInt16(ltonHP16(GameData::playerHP[GameData::playerType]))
        .writeInt16(ltonFP(GameData::playerFP[GameData::playerType]));
    networkDriver_->send(writer_.takePacket(), 0, ServerTypes::PacketType::PKT_LOGIN, true);
    writer_.clearBuffer();
}
void LevelSelectMenu::handleLevelButtonClick(int levelNum) {  //levelNum supposed to be in current level
    if (!hasLogin) {
        statusIndicator_.setStateProcessing();
    }
    else {
        shouldReloadUI = true;
        SceneSwitchRequest request = {
            SceneSwitchRequest::Push,
            std::make_unique<LevelScene1>(networkDriver_, playerAttributes_, chatBox_),
            0,
            0
        };
        onRequestSwitch_(request);
    }
}

