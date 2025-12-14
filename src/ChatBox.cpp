
#include "client/common/ChatBox.h"

#include <chrono>
#include <cmath>
#include <sstream>
#include <utility>

#include "client/common/ResourceManager.h"
#include "common/Types.h"
#include "common/network/ClientNetworkDriver.h"
#include "common/utils/Random.h"
constexpr int CHARACTER_SIZE = 40;
constexpr float TEXT_SCALE = 0.3f;
sf::Color ChatBox::getColor(char code) {
    switch (code) {
        case '0': return {0, 0, 0};       // Black
        case '1': return {0, 0, 170};     // Dark Blue
        case '2': return {0, 170, 0};     // Dark Green
        case '3': return {0, 170, 170};   // Dark Aqua
        case '4': return {170, 0, 0};     // Dark Red
        case '5': return {170, 0, 170};   // Dark Purple
        case '6': return {255, 170, 0};   // Gold
        case '7': return {170, 170, 170}; // Gray
        case '8': return {85, 85, 85};    // Dark Gray
        case '9': return {85, 85, 255};   // Blue
        case 'a': return {85, 255, 85};   // Green
        case 'b': return {85, 255, 255};   // Aqua
        case 'c': return {255, 85, 85};   // Red
        case 'd': return {255, 85, 255};   // Light Purple
        case 'e': return {255, 255, 85};   // Yellow
        case 'f': return sf::Color::White;          // White
        default: return sf::Color::White;
    }
}
ChatBox::ChatBox(ClientNetworkDriver& driver, std::string selfName)
    :font_(ResourceManager::getFont("fonts/font6.ttf")), driver_(driver), selfName_(std::move(selfName)){
    background_.setFillColor(sf::Color(0, 0, 0, 128));
    init();
}
void ChatBox::init() {
    inputText_.setFont(font_);
    inputText_.setCharacterSize(CHARACTER_SIZE);
    inputText_.setScale(TEXT_SCALE, TEXT_SCALE);
    inputText_.setFillColor(sf::Color::White);
    inputText_.setPosition(LEFT_OFFSET, 0);
}
void ChatBox::open() {
    isExpanded_ = true;
    scrollOffset_ = 0;
    for (auto& line : lines_) {
        line.alpha = 255.f;
    }
}
void ChatBox::close() {
    isExpanded_ = false;
    //inputBuffer_.clear();
    scrollOffset_ = 0;
}
void ChatBox::onMouseScroll(float delta) {
    if (!isExpanded_) return;
    if (delta > 0) {
        scrollOffset_++;
    } else {
        scrollOffset_--;
    }
    int maxScroll = std::max(0, (int)lines_.size() - MAX_MESSAGE_ON_SCREEN + 2);
    if (scrollOffset_ > maxScroll) scrollOffset_ = maxScroll;
    if (scrollOffset_ < 0) scrollOffset_ = 0;
}
void ChatBox::addMessage(const std::string& message) {
    parseAndWrap(message);
    while (lines_.size() > MAX_HISTORY) {
        lines_.pop_front();
    }
}
std::vector<std::string> ChatBox::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
ChatBox::ProcessedWord ChatBox::processWord(const std::string& word, StyleState& state) {
    ProcessedWord result;
    result.width = 0.f;
    std::string buffer;
    auto flushBuffer = [&]() {
        if (buffer.empty()) return;
        sf::Text text(buffer, font_, CHARACTER_SIZE);
        text.setScale(TEXT_SCALE, TEXT_SCALE);
        text.setFillColor(state.color);
        sf::Uint32 style = sf::Text::Regular;
        if (state.bold) style |= sf::Text::Bold;
        if (state.italic) style |= sf::Text::Italic;
        text.setStyle(style);
        float w = text.getGlobalBounds().width;
        result.chunks.push_back({text, state.obfuscated, buffer});
        result.width += w;
        buffer.clear();
    };
    for (size_t i = 0; i < word.size(); ++i) {
        if (word[i] == '&' && i + 1 < word.size()) {
            flushBuffer();
            char code = word[i + 1];
            bool isColor = (code >= '0' && code <= '9') || (code >= 'a' && code <= 'f');
            if (isColor) {
                state.color = getColor(code);
                state.bold = false;
                state.italic = false;
                state.obfuscated = false;
            }
            else {
                switch (code) {
                    case 'l': state.bold = true; break;
                    case 'o': state.italic = true; break;
                    case 'k': state.obfuscated = true; break;
                    case 'r':
                        state.color = sf::Color::White;
                        state.bold = false;
                        state.italic = false;
                        state.obfuscated = false;
                        break;
                    default: break;
                }
            }
            ++i;
        }
        else {
            buffer += word[i];
        }
    }
    flushBuffer();
    return result;
}
void ChatBox::parseAndWrap(const std::string& rawMsg) {
    std::vector<std::string> words = splitString(rawMsg, ' ');  //losing spaces
    ChatLine currentLine;
    float currentLineWidth = 0.f;
    StyleState currentState;
    sf::Text spaceText("  ", font_, CHARACTER_SIZE);
    sf::Uint32 style = sf::Text::Regular;
    style |= sf::Text::Italic; //don't know why regular space is so short
    spaceText.setStyle(style);
    spaceText.setScale(TEXT_SCALE, TEXT_SCALE);
    float spaceWidth = spaceText.getGlobalBounds().width;
    for (size_t i = 0; i < words.size(); ++i) {
        ProcessedWord pWord = processWord(words[i], currentState);
        if (currentLineWidth > 0 && (currentLineWidth + pWord.width) > CHAT_WIDTH) {  //>0 to avoid long word
            lines_.push_back(currentLine);
            currentLine = ChatLine();
            currentLineWidth = 0.f;
        }
        for (const auto& chunk : pWord.chunks) {
            currentLine.chunks.push_back(chunk);
        }
        currentLineWidth += pWord.width;
        if (i < words.size() - 1) {  // add space if not last word
            sf::Text sp("  ", font_, CHARACTER_SIZE);
            sp.setScale(TEXT_SCALE, TEXT_SCALE);
            // if (currentState.bold) style |= sf::Text::Bold;
            // if (currentState.italic) style |= sf::Text::Italic;
            sp.setStyle(style);
            currentLine.chunks.push_back({sp, false, "  "});
            currentLineWidth += spaceWidth;
        }
    }
    if (!currentLine.chunks.empty()) {
        lines_.push_back(currentLine);
    }
}
void ChatBox::update(float dt) {
    // magicTextTimer_ += dt;
    // if (magicTextTimer_ > 0.05f) {
    //     updateObfuscatedText();
    //     magicTextTimer_ = 0.f;
    // }
    updateObfuscatedText();
    if (!isExpanded_) {
        for (auto& line : lines_) {
            line.timeAlive += dt;
            if (line.timeAlive < MESSAGE_LIFE) {
                line.alpha = 255.f;
            } else if (line.timeAlive < MESSAGE_LIFE + FADE_TIME) {
                float progress = (line.timeAlive - MESSAGE_LIFE) / FADE_TIME;
                line.alpha = 255.f * (1.f - progress);
            } else {
                line.alpha = 0.f;
            }
        }
    } else {
        for (auto& line : lines_) {
            line.timeAlive += dt;
            line.alpha = 255.f;
        }
    }
}
void ChatBox::updateObfuscatedText() {
    for (auto& line : lines_) {
        for (auto& chunk : line.chunks) {
            if (chunk.isObfuscated) {
                std::string noisy = chunk.originalString;
                for (auto& c : noisy) {
                    if (c != ' ') {
                        c = static_cast<char>(33 + Random::randInt(0, 93));
                    }
                }
                chunk.textObj.setString(noisy);
            }
        }
    }
}
void ChatBox::render(sf::RenderWindow& window) {
    sf::View originalView = window.getView();
    sf::Vector2u windowSize = window.getSize();
    constexpr float VIRTUAL_WIDTH = 720.0f;
    float aspectRatio = static_cast<float>(windowSize.x) / windowSize.y;
    float virtualHeight = VIRTUAL_WIDTH / aspectRatio;
    sf::View uiView(sf::FloatRect(0, 0, VIRTUAL_WIDTH, virtualHeight));
    window.setView(uiView);
    float inputBaseY = virtualHeight - BOTTOM_OFFSET;
    if (isExpanded_) {
        float boxHeight = static_cast<float>(std::min(static_cast<int>(lines_.size()), MAX_MESSAGE_ON_SCREEN))
            * LINE_HEIGHT;
        background_.setSize(sf::Vector2f(CHAT_WIDTH + 20, boxHeight + 30));
        background_.setPosition(0, inputBaseY - boxHeight);
        window.draw(background_);
        bool showCursor = (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
            ).count() / 350) % 2;
        inputText_.setString(inputBuffer_ + (showCursor ? "_" : ""));
        inputText_.setPosition(LEFT_OFFSET, inputBaseY + 14.f);
        window.draw(inputText_);
    }
    float currentY = inputBaseY + 10.f;
    int startIndex = static_cast<int>(lines_.size()) - 1 - scrollOffset_;
    int visibleCount = 0;
    for (int i = startIndex; i >= 0; --i) {
        if (!isExpanded_ && lines_[i].alpha <= 1.f) continue;
        if (!isExpanded_ && visibleCount > COLLAPSED_MAX_MESSAGE_ON_SCREEN) break;
        if (isExpanded_ && visibleCount > MAX_MESSAGE_ON_SCREEN) break;
        auto& line = lines_[i];
        float drawX = LEFT_OFFSET;
        currentY -= LINE_HEIGHT;
        for (auto& chunk : line.chunks) {
            sf::Color originalColor = chunk.textObj.getFillColor();
            auto alpha = static_cast<sf::Uint8>(line.alpha);
            sf::Color shadowColor = sf::Color(63, 63, 63, alpha);
            chunk.textObj.setFillColor(shadowColor);
            chunk.textObj.setPosition(std::floor(drawX) + 2.f, std::floor(currentY) + 2.f);
            window.draw(chunk.textObj);
            originalColor.a = alpha;
            chunk.textObj.setFillColor(originalColor);
            chunk.textObj.setPosition(std::floor(drawX), std::floor(currentY));
            window.draw(chunk.textObj);
            // sf::Color c = chunk.textObj.getFillColor();
            // c.a = static_cast<sf::Uint8>(line.alpha);
            // chunk.textObj.setFillColor(c);
            // sf::Color shadowColor = sf::Color(63, 63, 63, c.a);
            // sf::Text shadow = chunk.textObj;
            // shadow.setFillColor(shadowColor);
            // shadow.setPosition(std::floor(drawX) + 2.f, std::floor(currentY) + 2.f);
            // window.draw(shadow);
            // chunk.textObj.setPosition(std::floor(drawX), std::floor(currentY));
            // window.draw(chunk.textObj);
            drawX += chunk.textObj.getGlobalBounds().width;
        }
        ++visibleCount;
    }
    window.setView(originalView);
}
void ChatBox::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            close();
            return;
        }
    }
    if (event.type == sf::Event::TextEntered) {
        if (!isOpen()) {
            if (event.text.unicode == 't' || event.text.unicode == 'T') {  //open chatbox
                open();
            }
            return;
        }
        if (event.text.unicode == 8) { // Backspace
            if (!inputBuffer_.empty()) inputBuffer_.pop_back();
        } else if (event.text.unicode == 13) { // Enter
            if (!inputBuffer_.empty()) {
                std::string str = "&f<" + selfName_ + "> " + inputBuffer_;
                packetWriter_.writeStr(str.data(), str.size());
                driver_.send(packetWriter_.takePacket(), 1, ServerTypes::PacketType::PKT_MESSAGE, true);
                packetWriter_.clearBuffer();
                inputBuffer_.clear();
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 128 && inputBuffer_.size() < MAX_MESSAGE_CHARACTER) {
            inputBuffer_ += static_cast<char>(event.text.unicode);
        }
    }
    if (event.type == sf::Event::MouseWheelScrolled && isOpen()) {
        onMouseScroll(event.mouseWheelScroll.delta);
    }
}