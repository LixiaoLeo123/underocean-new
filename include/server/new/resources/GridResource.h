//
// Created by 15201 on 11/27/2025.
//

#ifndef UNDEROCEAN_GRIDRESOURCE_H
#define UNDEROCEAN_GRIDRESOURCE_H
#include <vector>
#include "common/Types.h"
#include "server/core(deprecate)/GameData.h"
struct GridResource {
    struct Cell {
        std::vector<Entity> entities;
        bool isAOI{false};
    };
    std::vector<Entity> outOfBoundEntities;   //will be cleared by system
    float cellWidth_, cellHeight_;
    int cols_, rows_;   //do not include "extra area"
    std::vector<Cell> cells_;
    void init(int w, int h, int cols, int rows) {
        cols_ = cols;
        rows_ = rows;
        cellWidth_ = static_cast<float>(w) / static_cast<float>(cols_);
        cellHeight_ = static_cast<float>(h) / static_cast<float>(rows_);
        cells_.resize((cols + 2) * (rows + 2));
        for(auto& c : cells_) c.entities.reserve(CELL_INIT_RESERVATION);
    }
    void clear() {
        for(auto& c : cells_) {
            c.entities.clear();
            c.isAOI = false;
        }
        outOfBoundEntities.clear();
    }
    void insert(Entity e, float x, float y) {
        int c = static_cast<int>(x / cellWidth_);
        int r = static_cast<int>(y / cellHeight_);
        if (c >= -1 && c <= cols_ && r >= -1 && r <= rows_) {
            int idx = (r + 1) * (cols_ + 2) + (c + 1);
            cells_[idx].entities.push_back(e);
        }
        else {
            outOfBoundEntities.push_back(e);
        }
    }
    void setOnAOI(float x, float y) {   //set chunk include this pos isAOI = true (regarding GameData::ENTITY_SYNC_RADIUS)
        int originC = static_cast<int>(x / cellWidth_);
        int originR = static_cast<int>(y / cellHeight_);
        if (originC - GameData::ENTITY_SYNC_RADIUS_X < -1) originC = -1 + GameData::ENTITY_SYNC_RADIUS_X;
        else if (originC + GameData::ENTITY_SYNC_RADIUS_X > cols_) originC = cols_ - GameData::ENTITY_SYNC_RADIUS_X;
        if (originR - GameData::ENTITY_SYNC_RADIUS_Y < -1) originR = -1 + GameData::ENTITY_SYNC_RADIUS_Y;
        else if (originR + GameData::ENTITY_SYNC_RADIUS_Y > rows_) originR = rows_ - GameData::ENTITY_SYNC_RADIUS_Y;
        for (int dc = -GameData::ENTITY_SYNC_RADIUS_X; dc <= GameData::ENTITY_SYNC_RADIUS_X; ++dc) {
            for (int dr = -GameData::ENTITY_SYNC_RADIUS_Y; dr <= GameData::ENTITY_SYNC_RADIUS_Y; ++dr) {
                int c = originC + dc;
                int r = originR + dr;
                if (c >= -1 && c <= cols_ && r >= -1 && r <= rows_) {
                    int idx = (r + 1) * (cols_ + 2) + (c + 1);
                    cells_[idx].isAOI = true;
                }
            }
        }
    }
    [[nodiscard]] bool cellExistAt(int r, int c) const {
        int index = (r + 1) * (cols_ + 2) + (c + 1);
        return index >= 0 && index < cells_.size();
    }
    [[nodiscard]] Cell& cellAt(int r, int c) {
        return cells_[(r + 1) * (cols_ + 2) + (c + 1)];
    }
};
#endif //UNDEROCEAN_GRIDRESOURCE_H