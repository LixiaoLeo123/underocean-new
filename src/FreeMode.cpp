//
// Created by 15201 on 11/18/2025.
//

#define UPDATE_CHUNK
#include "server/core/states/FreeMode.h"

#include "client/common/ResourceManager.h"
#include "common/utils/Random.h"
#include "server/game/creatures/SmallFish.h"

FreeMode::FreeMode(sf::RenderWindow& contextWindow) :
    Level(contextWindow), fishFactory(WIDTH, HEIGHT), grid(ROWS, std::vector<std::vector<OldEntity*>>(COLS)) {
    background.setTexture(ResourceManager::getTexture("images/backgrounds/bg0/bg0.png"));
    pEntities.resize(GameData::FREEMODE_MAX_FISH);
    generateFishFactory();
    fishFactory.generateFish(pEntities);
    resizeView(contextWindow);
}
void FreeMode::generateFishFactory() {
    fishFactory.registerFishByFunc(1.0, []() {
        sf::Vector2f pos = sf::Vector2f(static_cast<float>(Random::randInt(0, WIDTH)), static_cast<float>(Random::randInt(0, HEIGHT)));
        SmallFishType type = Random::randType<SmallFishType>();
        //SmallFishType type = SmallFishType::TYPE0;
        const auto& params = SmallFish::getParamsForType(type);
        float size = Random::randFloat(params.minSize, params.maxSize);
        sf::Vector2f vel = Random::randUnitVector() * Random::randFloat(0.0f, params.maxVelocity);
        return std::make_unique<SmallFish>(pos, size, type, vel, ROWS, COLS, WIDTH, HEIGHT);
    });
}
void FreeMode::update(float dt) {
    for (auto& row : grid) {
        for (auto& cell : row) {
            cell.clear();  //avoid reallocating
        }
    }
    for (const auto& pEntity : pEntities) {  //divide into grid
        assert(pEntity);
        OldEntity* e = pEntity.get();
        const sf::Vector2f& pos = e->position;
        int col = static_cast<int>(pos.x / (WIDTH / static_cast<float>(COLS)));
        int row = static_cast<int>(pos.y / (HEIGHT / static_cast<float>(ROWS)));
        e->cachedCol = col;
        e->cachedRow = row;
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            grid[row][col].push_back(e);
        }
    }
#ifdef UPDATE_CHUNK
    for (int dr = -GameData::SIMULATING_DIST; dr <= GameData::SIMULATING_DIST; ++dr) {  //only tick nearby entities
        for (int dc = -GameData::SIMULATING_DIST; dc <= GameData::SIMULATING_DIST; ++dc) {
            int r = pEntities[0]->cachedRow + dr;
            int c = pEntities[0]->cachedCol + dc;
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
                for (OldEntity* pEntity : grid[r][c]) {
                    if (pEntity != pEntities[0].get())
                        pEntity->update(grid, dt);
                    else
                        pEntity->updateAsPlayer(dt, window);
                }
            }
        }
    }
#endif
#ifndef UPDATE_CHUNK
#define UPDATE_CHUNK
    for (auto& pEntity : pEntities) {
        pEntity->update(grid, dt);
    }
#endif
    for (std::unique_ptr<OldEntity>& pEntity : pEntities) {    //respawn
        if (pEntity->deleted) {
            pEntity = std::move(fishFactory.createFish());
        }
    }
    view.move(GameData::CAMERA_ALPHA * (pEntities[0]->position - view.getCenter()));
    correctViewRange();
}