//
// Created by 15201 on 12/15/2025.
//

#ifndef UNDEROCEAN_SKILLSYSTEM_H
#define UNDEROCEAN_SKILLSYSTEM_H
#include "ISystem.h"
#include "common/Types.h"

class SkillSystem : public ISystem {
public:
    void update(float dt) override;
    static SkillIndices getSkillIndices(EntityTypeID type);
};

inline void SkillSystem::update(float dt) {
}

inline SkillIndices SkillSystem::getSkillIndices(EntityTypeID type) {
    switch (type) {
        case EntityTypeID::SMALL_YELLOW:
            return {42, 2, 24, 1};
        default:
            return {0, 0, 0, 0};
    }
}
#endif //UNDEROCEAN_SKILLSYSTEM_H
