//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_SMALLFISH_H
#define UNDEROCEAN_SMALLFISH_H

#include "client/common/ResourceManager.h"
#include "common/utils/Random.h"
#include "server/game(deprecate)/OldEntity.h"

enum SmallFishType {
    TYPE0,
    TYPE1,
    TYPE2,
    TYPE3,
    TYPE4,
    TYPE5,
    TYPE6,
    TYPE7,
    COUNT
};
class SmallFish final : public OldEntity {
public:
    SmallFishType type;
    struct Params {
        float neighborRadius2;
        float separationRadius2;
        float avoidRadius2;
        float cohesionWeight;
        float separationWeight;
        float alignmentWeight;
        float avoidWeight;
        float maxVelocity;
        int spriteWidth;
        int spriteHeight;
        const std::string textureId;
        float minSize;
        float maxSize;
    };
    SmallFish(sf::Vector2f pos, float size, SmallFishType type = SmallFishType::TYPE0, sf::Vector2f velocity = {}, int ROWS = -1, int COLS = -1, int WIDTH = -1, int HEIGHT = -1, float rotation = 0.0f)
        : OldEntity(pos, size, velocity, rotation, ROWS, COLS), type(type), animTime(Random::randFloat(-10.0f, 10.0f)), cachedWIDTH(WIDTH), cachedHEIGHT(HEIGHT) {
        maxVelocity = getParams().maxVelocity;
        sprite.setTexture(ResourceManager::getTexture(getParams().textureId));
        textureRect = sf::IntRect(0, 0, getParams().spriteWidth, getParams().spriteHeight);
        sprite.setTextureRect(textureRect);
        sprite.setOrigin(static_cast<float>(getParams().spriteWidth / 2),
            static_cast<float>(getParams().spriteHeight / 2));
        sprite.setScale(0.4f, 0.4f);
        sprite.setPosition(position);
    }
    ~SmallFish() override = default;
    void update(const std::vector<std::vector<std::vector<OldEntity*>>>& grid, float dt) override;
    void updateAsPlayer(float dt, sf::RenderWindow& window) override;
    void draw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }
    const Params& getParams() const {
        return getParamsTable()[static_cast<size_t>(type)];
    }
    static const Params& getParamsForType(SmallFishType type) {
        return getParamsTable()[static_cast<size_t>(type)];
    }
protected:
    float animTime;
    static const std::array<Params, static_cast<size_t>(SmallFishType::COUNT)>& getParamsTable();
    int cachedWIDTH = -1;
    int cachedHEIGHT = -1;
    float maxVelScale = 0.4f;
};
#endif //UNDEROCEAN_SMALLFISH_H