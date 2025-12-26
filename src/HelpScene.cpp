#include "client/scenes/helpscene/HelpScene.h"

#include "client/scenes/startmenu/StartMenu.h"
#include "client/common/InputManager.h"
#include <cmath>
void HelpScene::HelpEntry::init(const std::string& key, const std::string& desc, float yPos, float width, float targetHeight) {
    auto& font = ResourceManager::getFont("fonts/font4.ttf");
    keyText.setFont(font);
    keyText.setString(key);
    keyText.setCharacterSize(64); // 统一设为 64
    keyText.setFillColor(sf::Color(0, 255, 255)); // Cyan 
    // keyText.setStyle(sf::Text::Bold);
    sf::FloatRect keyBounds = keyText.getLocalBounds();
    float keyH = keyBounds.height > 0 ? keyBounds.height : 1.f; 
    float scale = targetHeight / keyH;
    keyText.setScale(scale, scale);
    descText.setFont(font);
    descText.setString(desc);
    descText.setCharacterSize(64);
    descText.setFillColor(sf::Color(230, 230, 230));
    descText.setScale(scale, scale);
    sf::FloatRect scaledKeyBounds = keyText.getGlobalBounds();
    keyText.setPosition(width * 0.45f - scaledKeyBounds.width, yPos);
    descText.setPosition(width * 0.48f, yPos);
    separatorLine.setSize(sf::Vector2f(2.f, targetHeight * 0.8f));
    separatorLine.setOrigin(1.f, 0.f);
    separatorLine.setPosition(width * 0.465f, yPos + targetHeight * 0.1f);
    separatorLine.setFillColor(sf::Color(255, 255, 255, 100));
}
void HelpScene::HelpEntry::render(sf::RenderWindow& window) {
    window.draw(keyText);
    window.draw(descText);
    window.draw(separatorLine);
}
HelpScene::HelpScene(const std::shared_ptr<SmoothTextLabel>& title)
    : title_(title),
      font_(ResourceManager::getFont("fonts/font4.ttf")),
      bgObj0_(ResourceManager::getTexture("images/backgrounds/bg1/bubble0.png"), HEIGHT, -0.1f, 0.f, 10.f, 0.06f),
      bgObj1_(ResourceManager::getTexture("images/backgrounds/bg1/bubble1.png"), HEIGHT, 0.15f, 1.f, 10.f, 0.06f)
{
    background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg1/bg1_all.png"));
    title_->setVisible(true);
    title_->setOutlineColor(sf::Color::Black, 1);
    float boxWidth = WIDTH * 0.85f;
    float boxHeight = HEIGHT * 0.6f;
    contentBox_.setSize({boxWidth, boxHeight});
    contentBox_.setOrigin(boxWidth / 2.f, boxHeight / 2.f);
    contentBox_.setPosition(WIDTH / 2.f, HEIGHT * 0.62f);
    contentBox_.setFillColor(sf::Color(0, 0, 0, 160));
    contentBox_.setOutlineColor(sf::Color(255, 255, 255, 50));
    contentBox_.setOutlineThickness(1.f);
    float startY = contentBox_.getGlobalBounds().top + 20.f;
    float rowHeight = 22.0f;
    float gap = 42.0f;
    struct Data { std::string k; std::string d; };
    std::vector<Data> list = {
        {"MOUSE", "Move"},
        {"SPACE 1-3", "Cast Skills"},
        {"Q", "Back Pause"},
        {"T", "Chat"}
    };

    for (const auto& item : list) {
        HelpEntry entry;
        entry.init(item.k, item.d, startY, WIDTH, rowHeight);
        entries_.push_back(entry);
        startY += gap;
    }
    backButton_.setFont(font_);
    backButton_.setString("< BACK");
    backButton_.setCharacterSize(64);
    float btnTargetH = 18.f;
    float btnScale = btnTargetH / backButton_.getLocalBounds().height;
    backButton_.setScale(btnScale, btnScale);
    backButton_.setPosition(4.f, 4.f);
    backButton_.setFillColor(sf::Color(200, 200, 200));
}
void HelpScene::resetViewArea(unsigned winWidth, unsigned winHeight) {
    float windowRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);
    float designRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    sf::Vector2f viewSize;
    if (windowRatio > designRatio) {
        viewSize.y = HEIGHT;
        viewSize.x = HEIGHT * windowRatio;
    } else {
        viewSize.x = WIDTH;
        viewSize.y = WIDTH / windowRatio;
    }
    view_.setSize(viewSize);
    view_.setCenter(WIDTH / 2.f, HEIGHT / 2.f);
    const sf::Texture* tex = background_.getTexture();
    if (tex) {
        sf::Vector2u texSize = tex->getSize();
        float scaleX = viewSize.x / static_cast<float>(texSize.x);
        float scaleY = viewSize.y / static_cast<float>(texSize.y);
        float bgScale = std::max(scaleX, scaleY);
        background_.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
        background_.setScale(bgScale, bgScale);
        background_.setPosition(WIDTH / 2.f, HEIGHT / 2.f);
    }
    viewDirty_ = true;
}
void HelpScene::goBack() {
    SceneSwitchRequest request = {
        SceneSwitchRequest::Replace,
        std::make_unique<StartMenu>(title_, true), // 假设 StartMenu 构造函数支持
        0,
        2
    };
    onRequestSwitch_(request);
}
void HelpScene::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        resetViewArea(event.size.width, event.size.height);
    }
    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Q) {
            goBack();
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (isBackHovered_) {
                goBack();
            }
        }
    }
}
void HelpScene::update(float dt) {
    bgObj0_.update(dt);
    bgObj1_.update(dt);
    title_->updateTotal(dt);
    sf::Vector2f mousePos = InputManager::getInstance().mousePosWorld;
    if (backButton_.getGlobalBounds().contains(mousePos)) {
        isBackHovered_ = true;
        backButton_.setFillColor(sf::Color::White);
    } else {
        isBackHovered_ = false;
        backButton_.setFillColor(sf::Color(180, 180, 180));
    }
}
void HelpScene::render(sf::RenderWindow& window) {
    if (!viewInit_) {
        sf::Vector2u winSize(window.getSize());
        resetViewArea(winSize.x, winSize.y);
        sf::Vector2f titleSize = title_->calculateSizeByWidth(WIDTH * 0.5f);
        title_->convertTo(view_);
        title_->setBounds(sf::Vector2f((WIDTH - titleSize.x) / 2.f, HEIGHT * 0.01f), titleSize);
        viewInit_ = true;
    }
    if (viewDirty_) {
        viewDirty_ = false;
        window.setView(view_);
    }
    window.draw(background_);
    bgObj0_.render(window);
    bgObj1_.render(window);
    window.draw(contentBox_);
    for (auto& entry : entries_) {
        entry.render(window);
    }
    window.draw(backButton_);
    title_->render(window);
}
void HelpScene::handleSwitchRequest(SceneSwitchRequest& req) {
    if (req.action == SceneSwitchRequest::None && req.extra == 2) {
        title_->setOldView(view_);
    } else {
        INodeScene::handleSwitchRequest(req);
    }
}