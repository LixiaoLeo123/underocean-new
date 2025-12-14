#ifndef UNDEROCEAN_CHATBOX_H
#define UNDEROCEAN_CHATBOX_H

#include <deque>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "common/net(depricate)/PacketWriter.h"


class ClientNetworkDriver;
class ChatBox {
public:
    explicit ChatBox(ClientNetworkDriver& driver, std::string  selfName);
    void init();
    void addMessage(const std::string& message);
    void update(float dt);
    void render(sf::RenderWindow& window);
    void onMouseScroll(float delta);
    void handleEvent(const sf::Event& event);
    void open();
    void close();
    bool isOpen() const { return isExpanded_; }
private:
    struct TextChunk {
        sf::Text textObj;
        bool isObfuscated = false;
        std::string originalString;
    };
    struct ChatLine {
        std::vector<TextChunk> chunks;
        float timeAlive = 0.f;
        float alpha = 255.f;
        int originalMessageIndex = 0;
    };
    struct StyleState {
        sf::Color color = sf::Color::White;
        bool bold = false;
        bool italic = false;
        bool obfuscated = false;
    };
    struct ProcessedWord {
        std::vector<TextChunk> chunks;
        float width;
    };
    ProcessedWord processWord(const std::string& word, StyleState& state);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    void parseAndWrap(const std::string& rawMsg);
    [[nodiscard]] static sf::Color getColor(char code);
    void updateObfuscatedText();
private:
    std::deque<ChatLine> lines_;
    static constexpr size_t MAX_HISTORY = 100;
    static constexpr float CHAT_WIDTH = 600.f;
    static constexpr float LINE_HEIGHT = 20.f;
    static constexpr float MESSAGE_LIFE = 10.0f;
    static constexpr float FADE_TIME = 1.0f;
    static constexpr float BOTTOM_OFFSET = 35.f;
    static constexpr float LEFT_OFFSET = 10.f;
    static constexpr int MAX_MESSAGE_ON_SCREEN = 20;
    static constexpr int COLLAPSED_MAX_MESSAGE_ON_SCREEN = 10;
    ClientNetworkDriver& driver_;  //to send message
    PacketWriter packetWriter_{};  //reuse
    std::string selfName_;  //to add name prefix
    const sf::Font& font_;
    bool isExpanded_ = false;
    int scrollOffset_ = 0; //positive
    float magicTextTimer_ = 0.f;
    std::string inputBuffer_;
    sf::Text inputText_;
    sf::RectangleShape background_;
};

#endif //UNDEROCEAN_CHATBOX_H