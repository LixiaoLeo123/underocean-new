//
// Created by 15201 on 12/4/2025.
//

#ifndef UNDEROCEAN_LEVELSCENE1_H
#define UNDEROCEAN_LEVELSCENE1_H
#include "LevelSceneBase.h"
#include "server/new/component/Components.h"

class LevelScene1 : public LevelSceneBase {
public:
    explicit LevelScene1(const std::shared_ptr<ClientNetworkDriver>& driver)
        :LevelSceneBase(driver) {
        player.setBorder(MAP_SIZE.x, MAP_SIZE.y);
        player.setType(static_cast<EntityTypeID>(GameData::playerType));
        player.setSize(GameData::playerSize[GameData::playerType]);
        // if (GameData::firstPlay) {
        //     player.setPos(-1000.f, -1000.f);  //out of map to play animation
        //     state_ = State::ANIMATION;
        //     background_.
        // }
        // else {
        //     background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg4/bg4.png"));
        // }
        background_.setTexture(ResourceManager::getTexture("images/backgrounds/bg4/bg4.png"));
        writer_.writeInt8(static_cast<std::uint8_t>(1));  //to level 1
        driver_->send(writer_.takePacket(), 0, ServerTypes::PacketType::PKT_LEVEL_CHANGE, 1);
        writer_.clearBuffer();
    }
    void render(sf::RenderWindow& window) override {
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
    }
    std::uint16_t ltonX(float x) override {
        float norm = 1.f / (2 + CHUNK_COLS) + x / (MAP_SIZE.x) / (1 + 2.f / CHUNK_COLS);
        norm = std::clamp(norm, 0.f, 1.f);
        return static_cast<std::uint16_t>(std::round(norm * 65535.f));
    }
    float ntolX(std::uint16_t x) override {
        float norm = static_cast<float>(x) / 65535.f;
        float offset = 1.f / (2 + CHUNK_COLS);
        norm = (norm - offset) * (1 + 2.f / CHUNK_COLS);
        return norm * MAP_SIZE.x;
    }
    float ntolY(std::uint16_t y) override {
        float norm = static_cast<float>(y) / 65535.f;
        float offset = 1.f / (2 + CHUNK_ROWS);
        norm = (norm - offset) * (1 + 2.f / CHUNK_ROWS);
        return norm * MAP_SIZE.y;
    }
    std::uint16_t ltonY(float y) override {
        float norm = 1.f / (2 + CHUNK_ROWS) + y / (MAP_SIZE.y) / (1 + 2.f / CHUNK_ROWS);
        norm = std::clamp(norm, 0.f, 1.f);
        return static_cast<std::uint16_t>(std::round(norm * 65535.f));
    }
    UVector getMapSize() override { return MAP_SIZE; };
private:
    static constexpr UVector MAP_SIZE{1024.f, 192.f };  //decided by bg
    static constexpr int CHUNK_ROWS = 4;   //about 50 x 50 px
    static constexpr int CHUNK_COLS = 21;   //no chunk update needed on client, but for net pos convert
    enum class State {
        ANIMATION,
        GAMING
    };
    State state_ { State::GAMING };
};
#endif //UNDEROCEAN_LEVELSCENE1_H