//
// Created by 15201 on 11/18/2025.
//

#ifndef UNDEROCEAN_FISHFACTORY_H
#define UNDEROCEAN_FISHFACTORY_H
#include <cassert>
#include <functional>
#include <memory>
#include <random>

#include "OldEntity.h"


class FishFactory {
public:
    FishFactory(int Width, int  Height)
        :WIDTH(Width), HEIGHT(Height){}
    using Creator = std::function<std::unique_ptr<OldEntity>()>;
    template<typename FishType, typename... Args>
    void registerFish(double weight, Args&&... args) {   //weight must be positive, static arguments
        assert(weight >= 0.0);
        creators_.push_back([args...]() -> std::unique_ptr<OldEntity> {
            return std::make_unique<FishType>(args...);
         });
        weights_.push_back(weight);
    }
    void registerFishByFunc(double weight, Creator creator) {
        assert(weight >= 0.0);
        creators_.push_back(std::move(creator));
        weights_.push_back(weight);
    }
    void generateFish(std::vector<std::unique_ptr<OldEntity>>& pEntities) {
        for (auto& pEntity : pEntities) {
            if (!pEntity) {
                pEntity = std::move(createFish());
            }
        }
    }
    std::unique_ptr<OldEntity> createFish() {    //call after registerFish all done
        static std::discrete_distribution<size_t> dist;
        static bool first_call = true;
        if (first_call) {   //avoid init before registerFish
            dist = std::discrete_distribution<size_t>(weights_.begin(), weights_.end());
            first_call = false;
        }
        size_t index = dist(gen_);
        return creators_[index]();
    }
private:
    std::vector<Creator> creators_;
    std::vector<double> weights_;
    std::mt19937 gen_{ std::random_device{}() };
    int WIDTH;
    int HEIGHT;
};
#endif //UNDEROCEAN_FISHFACTORY_H